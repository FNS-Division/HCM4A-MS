#ifndef _HCM4AMobileService_Excel_h_
#define _HCM4AMobileService_Excel_h_

#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


//	std::vector<std::vector<std::string>> data = {
//		{"One1", "One2", "10"}, {"Two1", "Two2", "20"}, {"Three1", "Three2", "30"}};
//
//	std::vector<int> columnWidths = {20, 20, 10};
//
//	saveToXLSX(data, columnWidths, "output.xlsx");



void saveToXLSX(const std::vector<std::vector<std::string>>& data,
                const std::vector<int>& columnWidths,
                const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return;

    file << "<?xml version=\"1.0\"?>\n";
    file << "<Workbook xmlns=\"urn:schemas-microsoft-com:office:spreadsheet\"\n";
    file << "          xmlns:o=\"urn:schemas-microsoft-com:office:office\"\n";
    file << "          xmlns:x=\"urn:schemas-microsoft-com:office:excel\"\n";
    file << "          xmlns:ss=\"urn:schemas-microsoft-com:office:spreadsheet\">\n";
    

    file << "  <Styles>\n";
    file << "    <Style ss:ID=\"BoldStyle\">\n";
    file << "      <Font ss:Bold=\"1\"/>\n"; 
    file << "    </Style>\n";
    file << "  </Styles>\n";


    file << "  <Worksheet ss:Name=\"Sheet1\">\n";
    file << "    <Table>\n";
    

    for (size_t i = 0; i < columnWidths.size(); ++i) {
        file << "      <Column ss:Width=\"" << columnWidths[i] << "\"/>\n";
    }


    for (size_t rowIndex = 0; rowIndex < data.size(); ++rowIndex) {
        const auto& row = data[rowIndex];
        file << "      <Row>\n";
        for (const auto& cell : row) {
            if (rowIndex == 0) {
                file << "        <Cell ss:StyleID=\"BoldStyle\">"
                     << "<Data ss:Type=\"String\">" << cell << "</Data></Cell>\n";
            } else {
                file << "        <Cell><Data ss:Type=\"String\">" 
                     << cell << "</Data></Cell>\n";
            }
        }
        file << "      </Row>\n";
    }

    file << "    </Table>\n";
    file << "  </Worksheet>\n";
    file << "</Workbook>\n";
    file.close();
}


#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <ctime>

using namespace std;


void writeUInt16LE(ofstream& out, uint16_t value) {
    char bytes[2];
    bytes[0] = static_cast<char>(value & 0xFF);
    bytes[1] = static_cast<char>((value >> 8) & 0xFF);
    out.write(bytes, 2);
}

void writeUInt32LE(ofstream& out, uint32_t value) {
    char bytes[4];
    bytes[0] = static_cast<char>(value & 0xFF);
    bytes[1] = static_cast<char>((value >> 8) & 0xFF);
    bytes[2] = static_cast<char>((value >> 16) & 0xFF);
    bytes[3] = static_cast<char>((value >> 24) & 0xFF);
    out.write(bytes, 4);
}


string escapeXml(const string& input) {
    string output;
    output.reserve(input.size());
    for (char c : input) {
        switch (c) {
            case '&':  output += "&amp;"; break;
            case '<':  output += "&lt;";  break;
            case '>':  output += "&gt;";  break;
            case '"':  output += "&quot;"; break;
            case '\'': output += "&apos;"; break;
            default:   output += c;       break;
        }
    }
    return output;
}


string generateSheetXml(const vector<vector<string>>& data, const vector<int>& columnWidths) {
    ostringstream oss;
    oss << R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>)" << "\n"
        << R"(<worksheet xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main">)"
        << "\n  <cols>\n";
    
    for (size_t i = 0; i < columnWidths.size(); ++i) {
        oss << "    <col min=\"" << (i+1) << "\" max=\"" << (i+1) 
            << "\" width=\"" << columnWidths[i] << "\" customWidth=\"1\"/>\n";
    }
    
    oss << "  </cols>\n  <sheetData>\n";
    
    for (const auto& row : data) {
        oss << "    <row>\n";
        for (const auto& cell : row) {
            string escaped = escapeXml(cell);
            oss << "      <c><v>" << escaped << "</v></c>\n";
        }
        oss << "    </row>\n";
    }
    
    oss << "  </sheetData>\n</worksheet>";
    return oss.str();
}


uint32_t crc32(const string& data) {
    static uint32_t table[256];
    static bool initialized = false;
    
    if (!initialized) {
        for (uint32_t i = 0; i < 256; ++i) {
            uint32_t crc = i;
            for (int j = 0; j < 8; ++j) {
                crc = (crc >> 1) ^ ((crc & 1) ? 0xEDB88320 : 0);
            }
            table[i] = crc;
        }
        initialized = true;
    }
    
    uint32_t crc = 0xFFFFFFFF;
    for (char c : data) {
        crc = (crc >> 8) ^ table[(crc ^ static_cast<uint8_t>(c)) & 0xFF];
    }
    return crc ^ 0xFFFFFFFF;
}

void saveToXLSX3(const vector<vector<string>>& data,
                const vector<int>& columnWidths,
                const string& filename) {
                    
    string sheetXml = generateSheetXml(data, columnWidths);
    
    vector<tuple<string, string>> files = {
        {"[Content_Types].xml", R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">
    <Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>
    <Default Extension="xml" ContentType="application/xml"/>
    <Override PartName="/xl/workbook.xml" ContentType="application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml"/>
    <Override PartName="/xl/worksheets/sheet1.xml" ContentType="application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml"/>
</Types>)"},
        {"_rels/.rels", R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
    <Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" Target="xl/workbook.xml"/>
</Relationships>)"},
        {"xl/workbook.xml", R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<workbook xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main" xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships">
    <sheets>
        <sheet name="Sheet1" sheetId="1" r:id="rId1"/>
    </sheets>
</workbook>)"},
        {"xl/worksheets/sheet1.xml", sheetXml},
        {"xl/_rels/workbook.xml.rels", R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
    <Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet" Target="worksheets/sheet1.xml"/>
</Relationships>)"}
    };

    struct ZipFile {
        string name;
        string content;
        uint32_t crc;
        uint32_t size;
        uint32_t headerOffset;
    };
    
    vector<ZipFile> zipFiles;
    for (const auto& [name, content] : files) {
        zipFiles.push_back({
            name,
            content,
            crc32(content),
            static_cast<uint32_t>(content.size()),
            0
        });
    }

    ofstream out(filename, ios::binary);
    if (!out) return;

    for (auto& zf : zipFiles) {
        zf.headerOffset = static_cast<uint32_t>(out.tellp());
        
        writeUInt32LE(out, 0x04034b50);       
        writeUInt16LE(out, 20);               
        writeUInt16LE(out, 0);                
        writeUInt16LE(out, 0);                
        writeUInt16LE(out, 0);                
        writeUInt16LE(out, 0);                
        writeUInt32LE(out, zf.crc);           
        writeUInt32LE(out, zf.size);          
        writeUInt32LE(out, zf.size);          
        writeUInt16LE(out, zf.name.size());   
        writeUInt16LE(out, 0);                
        
        out.write(zf.name.c_str(), zf.name.size());
        out.write(zf.content.c_str(), zf.content.size());
    }


    uint32_t centralStart = static_cast<uint32_t>(out.tellp());
    
    for (const auto& zf : zipFiles) {
        writeUInt32LE(out, 0x02014b50);      
        writeUInt16LE(out, 20);              
        writeUInt16LE(out, 20);               
        writeUInt16LE(out, 0);                
        writeUInt16LE(out, 0);                
        writeUInt16LE(out, 0);                
        writeUInt16LE(out, 0);                
        writeUInt32LE(out, zf.crc);           
        writeUInt32LE(out, zf.size);         
        writeUInt32LE(out, zf.size);          
        writeUInt16LE(out, zf.name.size());  
        writeUInt16LE(out, 0);                
        writeUInt16LE(out, 0);                
        writeUInt16LE(out, 0);                
        writeUInt16LE(out, 0);                
        writeUInt32LE(out, 0);                
        writeUInt32LE(out, zf.headerOffset);  
        
        out.write(zf.name.c_str(), zf.name.size());
    }

    // Запись End of Central Directory
    uint32_t centralSize = static_cast<uint32_t>(out.tellp()) - centralStart;
    
    writeUInt32LE(out, 0x06054b50);           
    writeUInt16LE(out, 0);                    
    writeUInt16LE(out, 0);                    
    writeUInt16LE(out, zipFiles.size());      
    writeUInt16LE(out, zipFiles.size());     
    writeUInt32LE(out, centralSize);          
    writeUInt32LE(out, centralStart);         
    writeUInt16LE(out, 0);                    

    out.close();
}




void saveToXLSX2(const std::vector<std::vector<std::string>>& data,
				const std::vector<int>& columnWidths, const std::string& filename)
{
	if(data.empty()) {
		return;
	}

	std::time_t now = std::time(nullptr);
	std::tm* tm = std::localtime(&now);
	std::ostringstream dateStream;
	dateStream << std::put_time(tm, "%Y-%m-%dT%H:%M:%SZ");


	std::ofstream xlsxFile(filename, std::ios::binary);
	if(!xlsxFile.is_open()) {
		return;
	}

	// 1. [Content_Types].xml
	std::string contentTypes = R"(
		<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
		<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">
			<Default Extension="xml" ContentType="application/xml"/>
			<Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>
			<Override PartName="/xl/workbook.xml" ContentType="application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml"/>
			<Override PartName="/xl/worksheets/sheet1.xml" ContentType="application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml"/>
			<Override PartName="/xl/styles.xml" ContentType="application/vnd.openxmlformats-officedocument.spreadsheetml.styles+xml"/>
		</Types>
		)";

	// 2. workbook.xml
	std::string workbook = R"(
		<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
		<workbook xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main" xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships">
			<sheets>
				<sheet name="Sheet1" sheetId="1" r:id="rId1"/>
			</sheets>
		</workbook>
		)";

	// 3. styles.xml 
	std::string styles = R"(
		<?xml version="1.0" encoding="UTF-8"?>
		<styleSheet xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main">
			<fonts count="2">
				<font><sz val="12"/></font>
				<font><b val="1"/><sz val="12"/></font>
			</fonts>
			<cellXfs count="2">
				<xf applyFont="0"/> <!-- Обычный стиль -->
				<xf applyFont="1"/> <!-- Жирный стиль -->
			</cellXfs>
		</styleSheet>
		)";

		// 4. sheet1.xml 
	std::ostringstream sheetData;
			sheetData << R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
		<worksheet xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main">
			<cols>
		)";


	for(size_t i = 0; i < columnWidths.size(); ++i) {
		sheetData << "<col min=\"" << (i + 1) << "\" max=\"" << (i + 1) << "\" width=\""
				<< columnWidths[i] << "\" customWidth=\"1\"/>";
	}

		sheetData << R"(
		</cols>
		<sheetData>
	)";


	for(size_t rowIdx = 0; rowIdx < data.size(); ++rowIdx) {
		sheetData << "<row>";
		for(const auto& cell : data[rowIdx]) {
			if(rowIdx == 0) {
				sheetData << "<c s=\"1\"><v>" << escapeXml(cell)
						<< "</v></c>"; // s="1" → жирный стиль
			}
			else {
				sheetData << "<c><v>" << escapeXml(cell) << "</v></c>";
			}
		}
		sheetData << "</row>";
	}

	sheetData << R"(
		</sheetData>
	</worksheet>
	)";

	// 5. .rels 
	std::string rels = R"(
		<?xml version="1.0" encoding="UTF-8"?>
		<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
			<Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet" Target="worksheets/sheet1.xml"/>
			<Relationship Id="rId2" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles" Target="styles.xml"/>
		</Relationships>
		)";


	xlsxFile << "PK\x03\x04\x14\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
				"\x00\x00";
	xlsxFile << "[Content_Types].xml" << contentTypes;
	xlsxFile << "xl/workbook.xml" << workbook;
	xlsxFile << "xl/styles.xml" << styles;
	xlsxFile << "xl/worksheets/sheet1.xml" << sheetData.str();
	xlsxFile << "xl/_rels/workbook.xml.rels" << rels;

	xlsxFile.close();
}


#endif
