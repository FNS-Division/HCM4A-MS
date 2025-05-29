#include <CtrlLib/CtrlLib.h>
#include "GisMap.h"


using namespace Upp;

double TopoDistance(double LonA, double LatA, double LonB, double LatB) {
	/**
	* Calculates the distance between two geographical points using the Haversine formula.
	*
	* Parameters:
	* LonA (double): Longitude of the first point.
	* LatA (double): Latitude of the first point.
	* LonB (double): Longitude of the second point.
	* LatB (double): Latitude of the second point.
	*
	* Returns:
	* double: The distance between the two points in kilometers.
	*
	* Constraints:
	* - The function assumes valid coordinate values.
	*/
	const double R = 6371.29; // Earth radius in kilometers

	double delta_lat = LatB - LatA;
	double delta_lon = LonB - LonA;
	double a = pow(sin(delta_lat / 2.0 * M_PI / 180.0), 2) +
			cos(LatA * M_PI / 180.0) * cos(LatB * M_PI / 180.0) * pow(sin(delta_lon / 2.0 * M_PI / 180.0), 2);
	return 2.0 * R * atan2(sqrt(a), sqrt(1.0 - a));
}

void TopoGetStep(Pointf pnt1, double azimut, double distance_km, Pointf& pnt2) {
	/**
	* Calculates a new point given a starting point, azimuth, and distance.
	*
	* Parameters:
	* pnt1 (Pointf): The starting point.
	* azimut (double): The azimuth in degrees.
	* distance_km (double): The distance in kilometers.
	* pnt2 (Pointf&): The resulting point.
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	const double cPIdiv180 = M_PI / 180.0;
	const double c180divPI = 180.0 / M_PI;
	const double earthRadiusKm = 6371.0;

	// Convert degrees to radians
	double lat1 = pnt1.y * cPIdiv180;
	double lon1 = pnt1.x * cPIdiv180;
	double azimuth = azimut * cPIdiv180;

	// Convert distance from kilometers to radians
	double angularDistance = distance_km / earthRadiusKm;

	// Calculate the new latitude
	double lat2 = asin(sin(lat1) * cos(angularDistance) +
					cos(lat1) * sin(angularDistance) * cos(azimuth));

	// Calculate the new longitude
	double lon2 = lon1 + atan2(sin(azimuth) * sin(angularDistance) * cos(lat1),
							cos(angularDistance) - sin(lat1) * sin(lat2));

	// Convert radians back to degrees
	pnt2.y = lat2 * c180divPI;
	pnt2.x = lon2 * c180divPI;
}

String GetFilenameToTile(int x, int y, int zoom, bool isHybrid) {
	/**
	* Generates a filename for a map tile.
	*
	* Parameters:
	* x (int): The x coordinate of the tile.
	* y (int): The y coordinate of the tile.
	* zoom (int): The zoom level.
	* isHybrid (bool): Indicates if the tile is hybrid.
	*
	* Returns:
	* String: The generated filename.
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	String prefix = "";
	!isHybrid ? prefix = "Sputnic" : prefix = "Hybrid";
	String fname = GetExeFolder() + "\\Tiles" + prefix + "\\z" + AsString(zoom) +
									"\\x" + AsString(trunc(x / 1024)) +
									"\\y" + AsString(trunc(y / 1024)) +
									"\\" + AsString(x) + "." + AsString(y) + ".png";
	RealizeDirectory(GetFileFolder(fname));
	return fname;
}

Image GetGooleMapImage(int cx, int cy, int cz, bool isHybrid, Event<>& downloadComplete) {
	/**
	* Downloads a Google map tile image.
	*
	* Parameters:
	* cx (int): The x coordinate of the tile.
	* cy (int): The y coordinate of the tile.
	* cz (int): The zoom level.
	* isHybrid (bool): Indicates if the tile is hybrid.
	* downloadComplete (Event<>&): Event triggered when the download is complete.
	*
	* Returns:
	* Image: The downloaded map tile image.
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	String fname = GetFilenameToTile(cx, cy, cz, isHybrid);
	if (FileExists(fname)) {
		return StreamRaster::LoadFileAny(fname);
	}
	Thread().Run([=] {
		String request;
		if (isHybrid) {
			request << "http://mt0.google.com/vt/lyrs=h@169000000&hl=en&x=" << cx << "&y=" << cy << "&z=" << (cz);
		} else {
			request << "http://mt0.google.com/vt/lyrs=p@169000000&hl=en&x=" << cx << "&y=" << cy << "&z=" << (cz);
		}
		try {
			HttpRequest r(request);
			r.Timeout(5000);
			String h = r.Execute();
			if (r.IsSuccess()) {
				String fname = GetFilenameToTile(cx, cy, cz, isHybrid);
				FileOut f(fname);
				f.PutLine(r.GetContent());
				f.Close();
			}
			~r;
			downloadComplete();
		} catch (...) {
			// Handle exceptions
		}
	});
	return StreamRaster::LoadStringAny("");
}

Point CoordToTile(Pointf pnt, int zoom) {
	/**
	* Converts geographical coordinates to tile coordinates.
	*
	* Parameters:
	* pnt (Pointf): The geographical coordinates.
	* zoom (int): The zoom level.
	*
	* Returns:
	* Point: The tile coordinates.
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	Point r;
	int tileSize = 256;
	double initialResolution = 2 * M_PI * 6378137 / tileSize;
	double originShift = 2 * M_PI * 6378137 / 2.0;

	double mx = pnt.x * originShift / 180.0;
	double my = log(tan((90 + pnt.y) * M_PI / 360.0)) / (M_PI / 180.0);
	my = my * originShift / 180.0;

	double res = initialResolution / (pow(2, zoom));
	r.x = (int)((mx + originShift) / res / tileSize);
	r.y = (int)((originShift - my) / res / tileSize);
	return r;
}

Pointf TileToCoord(int cx, int cy, int zoom) {
	/**
	* Converts tile coordinates to geographical coordinates.
	*
	* Parameters:
	* cx (int): The x coordinate of the tile.
	* cy (int): The y coordinate of the tile.
	* zoom (int): The zoom level.
	*
	* Returns:
	* Pointf: The geographical coordinates.
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	Pointf coord;
	int tileSize = 256;
	double initialResolution = 2 * M_PI * 6378137 / tileSize;
	double originShift = 2 * M_PI * 6378137 / 2.0;

	double res = initialResolution / (pow(2, zoom));
	double mx = cx * tileSize * res - originShift;
	double my = originShift - cy * tileSize * res;

	coord.x = (mx / originShift) * 180.0;
	coord.y = (my / originShift) * 180.0;
	coord.y = 180 / M_PI * (2 * atan(exp(coord.y * M_PI / 180.0)) - M_PI / 2.0);
	return coord;
}

double TopoGetMetrsInPixel(double latitude, int zoom) {
	/**
	* Calculates the number of meters per pixel at a given latitude and zoom level.
	*
	* Parameters:
	* latitude (double): The latitude.
	* zoom (int): The zoom level.
	*
	* Returns:
	* double: The number of meters per pixel.
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	const double equator = 40075.016685578488;
	const double tileSize = equator * 1000 / 256;  //≈ 156543.03
	return tileSize * cos(0.017453292519943 * latitude) / pow(2, zoom);
}

Pointf GisMap::WebMercatorToXY(Pointf pnt) {
	/**
	* Converts Web Mercator coordinates to XY coordinates.
	*
	* Parameters:
	* pnt (Pointf): The Web Mercator coordinates.
	*
	* Returns:
	* Pointf: The XY coordinates.
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	Pointf result;
	result.x = pnt.x * 0.017453292519943 * 6378137.0;
	result.y = std::max(std::min(MAX_LATITUDE, pnt.y), -MAX_LATITUDE);
	result.y = log(tan(0.7853981633974 + (0.017453292519943 * result.y) / 2)) * 6378137.0;
	return Point(result.x, result.y);
}

Pointf GisMap::XyToWebMercator(Pointf pnt) {
	/**
	* Converts XY coordinates to Web Mercator coordinates.
	*
	* Parameters:
	* pnt (Pointf): The XY coordinates.
	*
	* Returns:
	* Pointf: The Web Mercator coordinates.
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	Pointf result;
	result.x = pnt.x / 6378137.0 * 57.295779513082323;
	result.y = (2 * atan(exp(pnt.y / 6378137.0)) - 1.570796326794896) * 57.295779513082323;
	return result;
}

Pointf GisMap::DegToPx(Pointf pnt) {
	/**
	* Converts degrees to pixel coordinates.
	*
	* Parameters:
	* pnt (Pointf): The geographical coordinates in degrees.
	*
	* Returns:
	* Pointf: The pixel coordinates.
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	pnt = Pointf(std::min(MAX_LONGITUDE, std::max(-MAX_LONGITUDE, pnt.x)), std::min(MAX_LATITUDE, std::max(-MAX_LATITUDE, pnt.y)));
	pnt = WebMercatorToXY(pnt);
	Pointf tmp = WebMercatorToXY(PointCenterGrad);
	Pointf result = Pointf((tmp.x - pnt.x) / metresIn1px, (pnt.y - tmp.y) / metresIn1px);
	return Pointf(-(result.x - PointCenterPx.x), (PointCenterPx.y - result.y));
}

Pointf GisMap::PxToDeg(Pointf pnt) {
	/**
	* Converts pixel coordinates to degrees.
	*
	* Parameters:
	* pnt (Pointf): The pixel coordinates.
	*
	* Returns:
	* Pointf: The geographical coordinates in degrees.
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	Pointf result = Pointf((pnt.x - PointCenterPx.x) * metresIn1px, (PointCenterPx.y - pnt.y) * metresIn1px);
	Pointf tmp2 = WebMercatorToXY(PointCenterGrad);
	tmp2 = XyToWebMercator(Pointf(result.x + tmp2.x, result.y + tmp2.y));
	return Pointf(std::min(MAX_LONGITUDE, std::max(-MAX_LONGITUDE, tmp2.x)), std::min(MAX_LATITUDE, std::max(-MAX_LATITUDE, tmp2.y)));
}

void GisMap::ZoomChange(int zm) {
	/**
	* Changes the zoom level of the map.
	*
	* Parameters:
	* zm (int): The new zoom level.
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	zoom = std::min(MAX_ZOOM, std::max(1, zm));
	metresIn1px = TopoGetMetrsInPixel(0, zoom);
	Refresh();
}

void GisMap::ToCenter() {
	/**
	* Centers the map view.
	*
	* Parameters:
	* None
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes the map is initialized.
	*/
	isDragging = false;
	PointCenterPx = Pointf(width / 2, height / 2);
	PointMouseUpPx = PointCenterPx;
	PointMouseDownPx = Pointf(0, 0);
}

void GisMap::LookAt(double longitude, double latitude, int zoom) {
	/**
	* Centers the map view on a specific location.
	*
	* Parameters:
	* longitude (double): The longitude to center on.
	* latitude (double): The latitude to center on.
	* zoom (int): The zoom level.
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	PointCenterGrad = Pointf(longitude, latitude);
	PointCenterPx = Pointf(width / 2, height / 2);
	ZoomChange(zoom);
	ToCenter();
}

void GisMap::MouseWheel(Point p, int zdelta, dword keyflags) {
	/**
	* Handles mouse wheel events to zoom the map.
	*
	* Parameters:
	* p (Point): The mouse position.
	* zdelta (int): The scroll delta.
	* keyflags (dword): The key flags.
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	flagDrawHeightMap = false;
	int zoom2 = std::clamp(zoom + sgn(zdelta), 1, 20);
	if (zoom2 != zoom) {
		Pointf pnt = PointMoveGrad;
		LookAt(pnt.x, pnt.y, zoom2);
	}
}

void GisMap::LeftDrag(Point pos, dword flags) {
	/**
	* Handles left mouse button drag events.
	*
	* Parameters:
	* pos (Point): The mouse position.
	* flags (dword): The key flags.
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	isDragging = true;
	PointMouseDownPx = pos;
	flagDrawHeightMap = false;
}

void GisMap::LeftUp(Point pos, dword flags) {
	/**
	* Handles left mouse button up events.
	*
	* Parameters:
	* pos (Point): The mouse position.
	* flags (dword): The key flags.
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	if (isDragging) {
		isDragging = false;
		PointMouseUpPx = Pointf(pos.x, pos.y);
		Pointf tmp = PxToDeg(Pointf(pos.x, pos.y));
		PointCenterPx = PointMouseUpPx;
		PointCenterGrad = tmp;
		PointMouseDownPx = PointMouseUpPx;
		Refresh();
	}
}

void GisMap::LeftDouble(Point pos, dword flags){
	PointCenterHeigthMap = pos;
	flagDrawHeightMap = true;
	Refresh();
}

void GisMap::MouseMove(Point p, dword keyflag) {
	/**
	* Handles mouse move events.
	*
	* Parameters:
	* p (Point): The mouse position.
	* keyflag (dword): The key flags.
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	PointMovePx = Pointf(p.x, p.y);
	PointMoveGrad = PxToDeg(PointMovePx);
	if (isDragging) {
		PointCenterPx.x = PointMouseUpPx.x - PointMouseDownPx.x + p.x;
		PointCenterPx.y = PointMouseUpPx.y - PointMouseDownPx.y + p.y;
		PointCenterGrad = PxToDeg(PointCenterPx);
		Refresh();
	}
	if (GetCtrl() || zoom > 8) {
		Pointf pnt = PointMoveGrad;
		lastTopo = ter.GetHeigth(pnt.x, pnt.y);
		lastMorpho = ter.GetMorpho(pnt.x, pnt.y);
	}
	if (GetShift()) {
		if (!isDistShowing) {
			PointDist = p;
			isDistShowing = true;
		}
		Refresh();
	} else {
		isDistShowing = false;
	}
}
void GisMap::DrawHeightMap(Draw& w){
	if (!flagDrawHeightMap) return;
	if (zoom <= 6) return;
		
	bool isMorpho = GetCtrl();
	Pointf pntNext, pntPx;
	
	Color color;
	int wid = 400;
	int hei = 400;
	
	for (int x = 0; x < wid; ++x){
		for (int y = 0; y < hei; ++y){
			pntNext = Pointf( PointCenterHeigthMap.x - trunc(wid/2) + x, PointCenterHeigthMap.y - trunc(hei/2) + y);
			pntPx = PxToDeg(pntNext);
			if (isMorpho){
				color = GetMorphoColor(ter.GetMorpho(pntPx.x, pntPx.y));
			}else{
				color = GetHeightColor(ter.GetHeigth(pntPx.x, pntPx.y));
			}
			w.DrawRect(pntNext.x, pntNext.y, 1, 1, color);
		}
	}
}

void GisMap::DrawProfile(Draw& w){
	/**
	* Draws the profile between two points on the map.
	*
	* Parameters:
	* w (Draw&): The drawing context.
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	if (!isDistShowing) return;
	if (width < 800) return;
	if (height < 600) return;
	Pointf pn1, pn2;
	pn1 = PxToDeg(PointDist);
	pn2 = PxToDeg(PointMovePx);
	
	ter.GetProfile(pn1.x, pn1.y, pn2.x, pn2.y);
	if (ter.M_Prof.size() == 0) return;
	//if (ter.M_Prof.size() < 500) return;
	
	int rBottom, rLeft, rRight, rTop, rWidth, rHeight, rPadding, rMarging, hMin, hMax;
	int fontW = w.GetStdFontSize().cx;
	rPadding = 10;
	rMarging = 10;
	rWidth = 300;
	rHeight = 187;
	rBottom = height - rPadding;
	rTop = rBottom - rHeight;
	rLeft = rPadding;
	rRight = rLeft + rWidth;
	hMin = round(*std::min_element(ter.T_Prof.begin(), ter.T_Prof.end()));
	hMin = std::min(0, hMin);
	hMax = round(*std::max_element(ter.T_Prof.begin(), ter.T_Prof.end()));
	
	w.DrawTextA(rLeft, rTop - 16, FormatDouble(pn1.x, 6) + " " + FormatDouble(pn1.y, 6));
	w.DrawTextA(rLeft, rTop - 32, FormatDouble(pn2.x, 6) + " " + FormatDouble(pn2.y, 6));
	
	String txt = "Min H=" + FormatDouble(hMin, 6) + " m";
	w.DrawTextA(rRight - txt.GetCount() * fontW - rMarging, rTop - 16, txt);
	txt = "Max H=" + FormatDouble(hMax, 6) + " m";
	w.DrawTextA(rRight - txt.GetCount() * fontW - rMarging, rTop - 32, txt);

	w.DrawRect(rLeft, rTop, rWidth, rHeight, White() );
	
	w.DrawLine(Point(rLeft, rBottom),Point(rLeft, rTop), 1, Black());
	w.DrawLine(Point(rLeft, rTop),Point(rRight, rTop), 1, Black());
	w.DrawLine(Point(rRight, rTop),Point(rRight, rBottom), 1, Black());
	w.DrawLine(Point(rRight, rBottom),Point(rLeft, rBottom), 1, Black());
	
	double kX = (rWidth * 1.0) / (ter.T_Prof.size() * 1.0);
	double kY = ((rHeight - rMarging * 2) * 1.0) / ((hMax - hMin)*1.0);

	Point pnt1, pnt2;
	for (int i=0; i < ter.T_Prof.size() - 1; ++i) {
		pnt1 = Point(i * kX + rLeft, rBottom - (ter.T_Prof[i] - hMin) * kY - rMarging);
		pnt2 = Point((i+1) * kX + rLeft, rBottom - (ter.T_Prof[i+1] - hMin) * kY - rMarging);
		w.DrawLine(pnt1,pnt2, 1, Black());
		
	}

}

void GisMap::DrawDistance(Draw& w) {
	/**
	* Draws the distance between two points on the map.
	*
	* Parameters:
	* w (Draw&): The drawing context.
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	if (!isDistShowing) return;
	w.DrawLine(PointDist, PointMovePx, 2, GrayColor(50));
	Pointf pn1, pn2;
	pn1 = PxToDeg(PointDist);
	pn2 = PxToDeg(PointMovePx);
	double d = TopoDistance(pn1.x, pn1.y, pn2.x, pn2.y);
	String txt =  AsString(round(d * 10) / 10) + " km";
	
	
	w.DrawRect(PointDist.x, PointDist.y, w.GetStdFontSize().cx * txt.GetCount(), w.GetStdFontSize().cy , White() );
	w.DrawTextA(PointDist.x, PointDist.y, txt );
}

void GisMap::DrawCountries(Draw& w) {
	/**
	* Draws country borders on the map.
	*
	* Parameters:
	* w (Draw&): The drawing context.
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	if (geodata.Borders.IsEmpty()) return;

	for (int i = 0; i < geodata.Borders.GetCount(); i++) {
		if (geodata.Borders[i].Distance != "ALL") continue;
		Vector<Point> vertices;

		vertices.SetCount(geodata.Borders[i].Points.GetCount());
		for (int j = 0; j < vertices.GetCount(); j++) {
			vertices[j] = DegToPx(geodata.Borders[i].Points[j]);
		}
		//w.DrawPolygon(vertices, GetRandomColor60(i), 1, Black());
		w.DrawPolygon(vertices, GetColorForCountry(geodata.Borders[i].CountryName), 1, Black());
	}
}

void GisMap::DrawLines(Draw& w) {
	/**
	* Draws latitude and longitude lines on the map.
	*
	* Parameters:
	* w (Draw&): The drawing context.
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	Pointf pnt1, pnt2;
	for (int lon = -180; lon <= 180; lon += 5) {
		pnt1 = DegToPx(Pointf(lon, -MAX_LATITUDE));
		pnt2 = DegToPx(Pointf(lon, MAX_LATITUDE));
		w.DrawLine(pnt1.x, pnt1.y, pnt2.x, pnt2.y, 1, Color(200, 200, 200));
	}
	for (int lat = -MAX_LATITUDE; lat <= MAX_LATITUDE; lat += 5) {
		pnt1 = DegToPx(Pointf(-MAX_LONGITUDE, lat));
		pnt2 = DegToPx(Pointf(MAX_LONGITUDE, lat));
		w.DrawLine(pnt1.x, pnt1.y, pnt2.x, pnt2.y, 1, Color(200, 200, 200));
	}
}

void GisMap::DrawTopoRect(bool isTopo, Draw& w) {
	/**
	* Draws rectangles for topographical or morphological data on the map.
	*
	* Parameters:
	* isTopo (bool): Indicates if the data is topographical.
	* w (Draw&): The drawing context.
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	if (geodata.TopoMorpho.IsEmpty()) {
		return;
	}
	int offset = isTopo ? 0 : 18;
	for (int i = 0; i < geodata.TopoMorpho.GetCount(); i++) {
		if (geodata.TopoMorpho[i].IsTopo != isTopo) { continue; }
		Vector<Point> vertices;
		vertices.SetCount(5);
		vertices[0] = Pointf(geodata.TopoMorpho[i].PointLD.x, geodata.TopoMorpho[i].PointLD.y);
		vertices[1] = Pointf(vertices[0].x + 1, vertices[0].y);
		vertices[2] = Pointf(vertices[0].x + 1, vertices[0].y + 1);
		vertices[3] = Pointf(vertices[0].x, vertices[0].y + 1);
		vertices[4] = vertices[0];

		vertices[0] = DegToPx(vertices[0]);
		vertices[1] = DegToPx(vertices[1]);
		vertices[2] = DegToPx(vertices[2]);
		vertices[3] = DegToPx(vertices[3]);
		vertices[4] = DegToPx(vertices[4]);

		w.DrawLine(vertices[0], vertices[1], Black());
		w.DrawLine(vertices[1], vertices[2], Black());
		w.DrawLine(vertices[2], vertices[3], Black());
		w.DrawLine(vertices[3], vertices[4], Black());
		w.DrawLine(vertices[4], vertices[0], Black());
		if (zoom > 8) {
			String txt = GetFileName(geodata.TopoMorpho[i].Filename);
			w.DrawTextA(vertices[0].x + round((vertices[2].x - vertices[0].x) / 2 - round(txt.GetCount() * 4 / 2)),
						vertices[0].y - round((vertices[0].y - vertices[3].y) / 2) - offset, txt);
		}
	}
}

void GisMap::DrawGoogle(Draw& w) {
	/**
	* Draws Google map tiles on the map.
	*
	* Parameters:
	* w (Draw&): The drawing context.
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	if (width == 0) return;
	if (zoom < 1) return;
	if (!options.showGoogle & !options.showGoogleHybrid) return;

	Pointf pnt1 = PxToDeg(Pointf(0, 0));
	pnt1 = CoordToTile(pnt1, zoom);
	Pointf pnt2 = PxToDeg(Pointf(width, height));
	pnt2 = CoordToTile(pnt2, zoom);

	Pointf pnt;
	Image img;
	for (int x = std::min(pnt1.x, pnt2.x) - 2; x <= std::max(pnt1.x, pnt2.x) + 2; x++) {
		for (int y = std::min(pnt1.y, pnt2.y) - 2; y <= std::max(pnt1.y, pnt2.y) + 2; y++) {
			img = GetGooleMapImage(x, y, zoom, options.showGoogleHybrid, downloadComplete);
			pnt = TileToCoord(x, y, zoom);
			pnt = DegToPx(pnt);
			if (!IsNull(img)) {
				w.DrawImage(pnt.x, pnt.y, img);
				img.Clear();
			}
		}
	}
	~img;
}

void GisMap::DrawStations(Draw& w) {
	/**
	* Draws stations on the map.
	*
	* Parameters:
	* w (Draw&): The drawing context.
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	if (width == 0) return;
	if (zoom < 2) return;

	Pointf pnt1, pnt2;
	int sizeObj, maxX, minX, maxY, minY, x;
	zoom < 7 ? sizeObj = 3 : sizeObj = 5;

	Vector<Point> vertices;
	vertices.SetCount(4);

	for (x = 0; x < MapStation.GetCount(); x++) {
		pnt1 = Pointf(MapStation[x].Longitude, MapStation[x].Latitude);

		if (MapStation[x].Radius > 0) {
			TopoGetStep(pnt1, 0, MapStation[x].Radius, pnt2);
			minY = trunc(DegToPx(pnt2).y);
			TopoGetStep(pnt1, 90, MapStation[x].Radius, pnt2);
			maxX = trunc(DegToPx(pnt2).x);
			TopoGetStep(pnt1, 180, MapStation[x].Radius, pnt2);
			maxY = trunc(DegToPx(pnt2).y);
			TopoGetStep(pnt1, 270, MapStation[x].Radius, pnt2);
			minX = trunc(DegToPx(pnt2).x);

			TopoGetStep(pnt1, 225, MapStation[x].Radius * 1.414, pnt2);
			pnt2 = DegToPx(pnt2);
			w.DrawArc(RectC(minX, minY, abs(maxY - minY), abs(maxY - minY)), Point(0, 0), Point(0, 0), 2, Cyan);
		} else {
			TopoGetStep(pnt1, MapStation[x].Azimut, 3, pnt2);
			pnt2 = DegToPx(pnt2);
			pnt1 = DegToPx(Pointf(MapStation[x].Longitude, MapStation[x].Latitude));
			w.DrawLine(pnt1.x, pnt1.y, pnt2.x, pnt2.y, 2, Black());
		}

		pnt1 = DegToPx(Pointf(MapStation[x].Longitude, MapStation[x].Latitude));

		vertices[0] = Pointf(pnt1.x - sizeObj, pnt1.y + sizeObj);
		vertices[1] = Pointf(pnt1.x, pnt1.y - sizeObj);
		vertices[2] = Pointf(pnt1.x + sizeObj, pnt1.y + sizeObj);
		vertices[3] = vertices[0];

		w.DrawPolyline(vertices, 2, MapStation[x].isRef ? Color(192, 0, 50) : Color(50, 0, 192));

		if (zoom > 7) w.DrawTextA(vertices[2].x - MapStation[x].Name.GetLength() * 5 / 2, vertices[2].y, MapStation[x].Name);
	}
}

void GisMap::DrawOrder(Draw& w) {
	/**
	* Draws the map layers in order.
	*
	* Parameters:
	* w (Draw&): The drawing context.
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	DrawCountries(w);

	if (!isDragging) DrawGoogle(w);

	if (options.showDegreeLines) DrawLines(w);
	if (options.showTopo) DrawTopoRect(true, w);
	if (options.showMorpho) DrawTopoRect(false, w);

	DrawHeightMap(w);
	
	DrawProfile(w);
	
	DrawStations(w);
	DrawDistance(w);
	
}

void GisMap::CheckMapCenter() {
	/**
	* Checks and adjusts the map center if it is out of bounds.
	*
	* Parameters:
	* None
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes the map is initialized.
	*/
	Pointf PntCenter = PxToDeg(Pointf(width / 2, height / 2));
	if (PntCenter.x == MAX_LONGITUDE || PntCenter.x == -MAX_LONGITUDE || PntCenter.y == MAX_LATITUDE || PntCenter.y == -MAX_LATITUDE) {
		ToCenter();
	}
}

void GisMap::Paint(Draw& w) {
	/**
	* Paints the map on the drawing context.
	*
	* Parameters:
	* w (Draw&): The drawing context.
	*
	* Returns:
	* void
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	Size sz = GetSize();
	width = sz.cx;
	height = sz.cy;
	if (width <= 0) return;
	w.DrawRect(sz, SWhiteGray());
	CheckMapCenter();
	DrawOrder(w);
}

double GisMap::RoundDigit(double digit, int places) {
	/**
	* Rounds a number to a specified number of decimal places.
	*
	* Parameters:
	* digit (double): The number to round.
	* places (int): The number of decimal places.
	*
	* Returns:
	* double: The rounded number.
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	places = pow(10, std::min(8, std::max(0, places)));
	return round(digit * places) / places;
}


Color GisMap::GetHeightColor(double height) {
	if (height <= 0) {
		return Color(0, 0, 255); // Голубой (вода)
	} else if (height < 100) {
		double t = height / 100.0;
		return Color(static_cast<unsigned char>(0 + t * 0),
					static_cast<unsigned char>(255 - t * 0),
					static_cast<unsigned char>(0 + t * 0)); // Зелёный → Жёлтый
	} else if (height < 500) {
		double t = (height - 100) / 400.0;
		return Color(static_cast<unsigned char>(255 - t * 0),
					static_cast<unsigned char>(255 - t * 155),
					static_cast<unsigned char>(0 + t * 0)); // Жёлтый → Оранжевый
	} else if (height < 1500) {
		double t = (height - 500) / 1000.0;
		return Color(static_cast<unsigned char>(255 - t * 116),
					static_cast<unsigned char>(165 - t * 96),
					static_cast<unsigned char>(0 + t * 19)); // Оранжевый → Коричневый
	} else if (height < 3000) {
		double t = (height - 1500) / 1500.0;
		return Color(static_cast<unsigned char>(139 - t * 0),
					static_cast<unsigned char>(69 - t * 69),
					static_cast<unsigned char>(19 + t * 236)); // Коричневый → Белый
	} else {
		return Color(255, 255, 255); // Белый (снежные вершины)
	}
}
Color GisMap::GetMorphoColor(int moprho) {
						
	switch(moprho)
	{
		case 0: return Color(198, 99, 0);  //land 
		case 1: return Color(0, 0, 255);   // sea
		case 2: return Color(128, 255, 255); //lake
		case 4: return Color(128, 255, 0); //coastal
		case 16: return Color(192, 192, 192); //buildings
		case 32: return Color(0, 255, 0); //trees
		case 64: return Color(255, 128, 0);
		case 128: return Color(255, 0, 0);
		default: return Color(0, 0, 0);
	}
}

Color GisMap::GetColorForCountry(const String& country) {

	static const std::unordered_map<int, Color> typeColors = {
		{1, Color(255, 182, 193)},  // Pastel Red 
		{2, Color(152, 251, 152)},  // Pastel Green 
		{3, Color(255, 209, 255)},  // Pastel Pink
		{4, Color(189, 186, 45)}  // Pastel Yellow 
	};

	static const std::unordered_map<std::string, int> countryTypes = {
		{"GNB", 1}, {"LBR", 1}, {"GHA", 1}, {"NGR", 1}, {"CME", 1}, {"SDN", 1}, {"COD", 1}, {"KEN", 1}, {"MWI", 1}, {"BOT", 1}, {"SWZ", 1},
		{"ALG", 2}, {"EGY", 2}, {"SEN", 2}, {"TCD", 2}, {"CTI", 2}, {"BEN", 2}, {"ETH", 2}, {"AGL", 2}, {"TZA", 2}, {"ZWE", 2}, {"AO1", 2}, {"TZ1", 2},
		{"MTN", 3}, {"TUN", 3}, {"GUI", 3}, {"BFA", 3}, {"NIG", 3}, {"SSD", 3}, {"DJI", 3}, {"GNE", 3}, {"COG", 3}, {"RRW", 3}, {"ZMB", 3}, {"AFS", 3}, {"GQ1", 3},
		{"MRC", 4}, {"LBY", 4}, {"MLI", 4}, {"GMB", 4}, {"ERI", 4}, {"SRL", 4}, {"TGO", 4}, {"CAF", 4}, {"SOM", 4}, {"GAB", 4}, {"UGA", 4}, {"BDI", 4}, {"NMB", 4}, {"MOZ", 4}, {"LSO", 4}, {"ZA1", 4}
	};


	auto it = countryTypes.find(country.ToStd());
	if (it != countryTypes.end()) {
		int type = it->second;
		return typeColors.at(type);
	}

    return GetRandomColor60( country.GetCount() < 2 ? 0 : static_cast<int>(country[0]) * static_cast<int>(country[1]) );

}

Color GisMap::GetRandomColor60(int Seed) {
	/**
	* Generates a random color with a lightness of 60%.
	*
	* Parameters:
	* Seed (int): The seed for the random color generator.
	*
	* Returns:
	* Color: The generated color.
	*
	* Constraints:
	* - The function assumes valid input values.
	*/
	double C, X, M;
	double R = 0, G = 0, B = 0;
	double Hue, Saturation, Lightness;

	if (Seed < 0) {
		Seed = Random(16777214);
	}

	Seed = Seed + 1;
	Seed = Seed + Seed * 15 - 1;
	Hue = fmod(Seed * 6, 360);
	Saturation = 1;
	Lightness = 0.60 + (Seed % 20) * 0.01;

	Hue = Hue / 60;
	C = (1 - fabs(2 * Lightness - 1)) * Saturation;
	X = C * (1 - fabs(Hue - floor(Hue) - 1));
	M = Lightness - C / 2;

	switch (static_cast<int>(Hue)) {
		case 0: R = C; G = X; B = 0; break;
		case 1: R = X; G = C; B = 0; break;
		case 2: R = 0; G = C; B = X; break;
		case 3: R = 0; G = X; B = C; break;
		case 4: R = X; G = 0; B = C; break;
		case 5: R = C; G = 0; B = X; break;
		default: R = 0; G = 0; B = 0; break;
	}

	return Color(
		static_cast<int>((R + M) * 255),
		static_cast<int>((G + M) * 255),
		static_cast<int>((B + M) * 255)
	);
}


GisMap::GisMap() {
	/**
	* Constructor for GisMap.
	* Initializes the map view.
	*/
	ToCenter();
	ZoomChange(5);
	

	downloadComplete << [=] {
		Time tt = GetSysTime();
		if (abs(tt.second - latTimeUpdate.second) >= 1) {
			Call([=] {
				Refresh();
			});
		}
		latTimeUpdate = tt;
	};
}
