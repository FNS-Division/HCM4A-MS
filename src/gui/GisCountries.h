#ifndef _HCM4AMobileService_gisCountries_h_
#define _HCM4AMobileService_gisCountries_h_


/**
 * Represents a country with its name and geographical coordinates.
 *
 * Parameters:
 * name (std::string): The name of the country.
 * latitude (double): The latitude of the country.
 * longitude (double): The longitude of the country.
 *
 * Constraints:
 * - The coordinates should be in a valid format.
 */
 
struct CountryWithCoord {
    std::string name;
    double latitude;
    double longitude;
};


std::vector<CountryWithCoord> africa = {
    {"Algeria", 28.033886, 1.659626},
    {"Angola", -11.202692, 17.873887},
    {"Benin", 9.30769, 2.315834},
    {"Botswana", -22.328474, 24.684866},
    {"Burkina Faso", 12.238333, -1.561593},
    {"Burundi", -3.373056, 29.918886},
    {"Cabo Verde", 16.5388, -23.0418},
    {"Cameroon", 7.369722, 12.354722},
    {"Central African Republic", 6.611111, 20.939444},
    {"Chad", 15.454166, 18.732207},
    {"Comoros", -11.875001, 43.872219},
    {"Congo (Congo-Brazzaville)", -0.228021, 15.827659},
    {"Côte d'Ivoire", 7.539989, -5.54708},
    {"Djibouti", 11.825138, 42.590275},
    {"Egypt", 26.820553, 30.802498},
    {"Equatorial Guinea", 1.650801, 10.267895},
    {"Eritrea", 15.179384, 39.782334},
    {"Eswatini (fmr. Swaziland)", -26.522503, 31.465866},
    {"Ethiopia", 9.145, 40.489673},
    {"Gabon", -0.803689, 11.609444},
    {"Gambia", 13.443182, -15.310139},
    {"Ghana", 7.946527, -1.023194},
    {"Guinea", 9.945587, -9.696645},
    {"Guinea-Bissau", 11.803749, -15.180413},
    {"Kenya", -0.023559, 37.906193},
    {"Lesotho", -29.609988, 28.233608},
    {"Liberia", 6.428055, -9.429499},
    {"Libya", 26.3351, 17.228331},
    {"Madagascar", -18.766947, 46.869107},
    {"Malawi", -13.254308, 34.301525},
    {"Mali", 17.570692, -3.996166},
    {"Mauritania", 21.00789, -10.940835},
    {"Mauritius", -20.348404, 57.552152},
    {"Morocco", 31.791702, -7.09262},
    {"Mozambique", -18.665695, 35.529562},
    {"Namibia", -22.95764, 18.49041},
    {"Niger", 17.607789, 8.081666},
    {"Nigeria", 9.081999, 8.675277},
    {"Rwanda", -1.940278, 29.873888},
    {"Sao Tome and Principe", 0.18636, 6.613081},
    {"Senegal", 14.497401, -14.452362},
    {"Seychelles", -4.679574, 55.491977},
    {"Sierra Leone", 8.460555, -11.779889},
    {"Somalia", 5.152149, 46.199616},
    {"South Africa", -30.559482, 22.937506},
    {"South Sudan", 6.876991, 31.306978},
    {"Sudan", 12.862807, 30.217636},
    {"Tanzania", -6.369028, 34.888822},
    {"Togo", 8.619543, 0.824782},
    {"Tunisia", 33.886917, 9.537499},
    {"Uganda", 1.373333, 32.290275},
    {"Zambia", -13.133897, 27.849332},
    {"Zimbabwe", -19.015438, 29.154857}
};


std::vector<CountryWithCoord> asia = {
    {"Afghanistan", 33.93911, 67.709953},
    {"Armenia", 40.069099, 45.038189},
    {"Azerbaijan", 40.143105, 47.576927},
    {"Bahrain", 26.0667, 50.5577},
    {"Bangladesh", 23.684994, 90.356331},
    {"Bhutan", 27.514162, 90.433601},
    {"Brunei", 4.535277, 114.727669},
    {"Cambodia", 12.565679, 104.990963},
    {"China", 35.86166, 104.195397},
    {"Cyprus", 35.126413, 33.429859},
    {"Georgia", 42.315407, 43.356892},
    {"India", 20.593684, 78.96288},
    {"Indonesia", -0.789275, 113.921327},
    {"Iran", 32.427908, 53.688046},
    {"Iraq", 33.223191, 43.679291},
    {"Israel", 31.046051, 34.851612},
    {"Japan", 36.204824, 138.252924},
    {"Jordan", 30.585164, 36.238414},
    {"Kazakhstan", 48.019573, 66.923684},
    {"Kuwait", 29.31166, 47.481766},
    {"Kyrgyzstan", 41.20438, 74.766098},
    {"Laos", 19.85627, 102.495496},
    {"Lebanon", 33.854721, 35.862285},
    {"Malaysia", 4.210484, 101.975766},
    {"Maldives", 3.202778, 73.22068},
    {"Mongolia", 46.862496, 103.846656},
    {"Myanmar (formerly Burma)", 21.916221, 95.955974},
    {"Nepal", 28.394857, 84.124008},
    {"North Korea", 40.339852, 127.510093},
    {"Oman", 21.512583, 55.923255},
    {"Pakistan", 30.375321, 69.345116},
    {"Palestine", 31.9522, 35.2332},
    {"Philippines", 12.879721, 121.774017},
    {"Qatar", 25.354826, 51.183884},
    {"Russia", 61.52401, 105.318756},
    {"Saudi Arabia", 23.885942, 45.079162},
    {"Singapore", 1.352083, 103.819836},
    {"South Korea", 35.907757, 127.766922},
    {"Sri Lanka", 7.873054, 80.771797},
    {"Syria", 34.802075, 38.996815},
    {"Tajikistan", 38.861034, 71.276093},
    {"Thailand", 15.870032, 100.992541},
    {"Timor-Leste", -8.874217, 125.727539},
    {"Turkmenistan", 38.969719, 59.556278},
    {"United Arab Emirates", 23.424076, 53.847818},
    {"Uzbekistan", 41.377491, 64.585262},
    {"Vietnam", 14.058324, 108.277199},
    {"Yemen", 15.552727, 48.516388}
};

std::vector<CountryWithCoord> europe = {
    {"Albania", 41.153332, 20.168331},
    {"Andorra", 42.506285, 1.521801},
    {"Austria", 47.516231, 14.550072},
    {"Belarus", 53.709807, 27.953389},
    {"Belgium", 50.503887, 4.469936},
    {"Bosnia and Herzegovina", 43.915886, 17.679076},
    {"Bulgaria", 42.733883, 25.48583},
    {"Croatia", 45.1, 15.2},
    {"Cyprus", 35.126413, 33.429859},
    {"Czechia (Czech Republic)", 49.817492, 15.472962},
    {"Denmark", 56.26392, 9.501785},
    {"Estonia", 58.595272, 25.013607},
    {"Finland", 61.92411, 25.748151},
    {"France", 46.227638, 2.213749},
    {"Germany", 51.165691, 10.451526},
    {"Greece", 39.074208, 21.824312},
    {"Hungary", 47.162494, 19.503304},
    {"Iceland", 64.963051, -19.020835},
    {"Ireland", 53.41291, -8.24389},
    {"Italy", 41.87194, 12.56738},
    {"Latvia", 56.879635, 24.603189},
    {"Liechtenstein", 47.166, 9.555373},
    {"Lithuania", 55.169438, 23.881275},
    {"Luxembourg", 49.815273, 6.129583},
    {"Malta", 35.937496, 14.375416},
    {"Moldova", 47.411631, 28.369885},
    {"Monaco", 43.738418, 7.424616},
    {"Montenegro", 42.708678, 19.37439},
    {"Netherlands", 52.132633, 5.291266},
    {"North Macedonia", 41.608635, 21.745275},
    {"Norway", 60.472024, 8.468946},
    {"Poland", 51.919438, 19.145136},
    {"Portugal", 39.399872, -8.224454},
    {"Romania", 45.943161, 24.96676},
    {"Russia", 61.52401, 105.318756},
    {"San Marino", 43.94236, 12.457777},
    {"Serbia", 44.016521, 21.005859},
    {"Slovakia", 48.669026, 19.699024},
    {"Slovenia", 46.151241, 14.995463},
    {"Spain", 40.463667, -3.74922},
    {"Sweden", 60.128161, 18.643501},
    {"Switzerland", 46.818188, 8.227512},
    {"Ukraine", 48.379433, 31.16558},
    {"United Kingdom", 55.378051, -3.435973},
    {"Vatican City", 41.902916, 12.453389}
};

std::vector<CountryWithCoord> northAmerica = {
    {"Antigua and Barbuda", 17.060816, -61.796428},
    {"Bahamas", 25.03428, -77.39628},
    {"Barbados", 13.193887, -59.543198},
    {"Belize", 17.189877, -88.49765},
    {"Canada", 56.130366, -106.346771},
    {"Costa Rica", 9.748917, -83.753428},
    {"Cuba", 21.521757, -77.781167},
    {"Dominica", 15.414999, -61.370976},
    {"Dominican Republic", 18.735693, -70.162651},
    {"El Salvador", 13.794185, -88.89653},
    {"Grenada", 12.1165, -61.678999},
    {"Guatemala", 15.783471, -90.230759},
    {"Haiti", 18.971187, -72.285215},
    {"Honduras", 15.199999, -86.241905},
    {"Jamaica", 18.109581, -77.297508},
    {"Mexico", 23.634501, -102.552784},
    {"Nicaragua", 12.865416, -85.207229},
    {"Panama", 8.537981, -80.782127},
    {"Saint Kitts and Nevis", 17.357822, -62.782998},
    {"Saint Lucia", 13.909444, -60.978893},
    {"Saint Vincent and the Grenadines", 12.984305, -61.287228},
    {"Trinidad and Tobago", 10.691803, -61.222503},
    {"United States of America", 37.09024, -95.712891}
};

std::vector<CountryWithCoord> southAmerica = {
    {"Argentina", -38.416097, -63.616672},
    {"Bolivia", -16.290154, -63.588653},
    {"Brazil", -14.235004, -51.92528},
    {"Chile", -35.675147, -71.542969},
    {"Colombia", 4.570868, -74.297333},
    {"Ecuador", -1.831239, -78.183406},
    {"Guyana", 4.860416, -58.93018},
    {"Paraguay", -23.442503, -58.443832},
    {"Peru", -9.189967, -75.015152},
    {"Suriname", 3.919305, -56.027783},
    {"Uruguay", -32.522779, -55.765835},
    {"Venezuela", 6.42375, -66.58973}
};

std::vector<CountryWithCoord> oceania = {
    {"Australia", -25.274398, 133.775136},
    {"Fiji", -17.713371, 178.065032},
    {"Kiribati", -3.370417, -168.734039},
    {"Marshall Islands", 7.131474, 171.184478},
    {"Micronesia", 7.425554, 150.550812},
    {"Nauru", -0.522778, 166.931503},
    {"New Zealand", -40.900557, 174.885971},
    {"Palau", 7.51498, 134.58252},
    {"Papua New Guinea", -6.314993, 143.95555},
    {"Samoa", -13.759029, -172.104629},
    {"Solomon Islands", -9.64571, 160.156194},
    {"Tonga", -21.178986, -175.198242},
    {"Tuvalu", -7.109535, 177.64933},
    {"Vanuatu", -15.376706, 166.959158}
};


#endif
