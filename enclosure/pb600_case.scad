// PB600 Sound Carrier: Gehäuse (Unterteil + Deckel), 3D-druckbar
// Koordinaten wie in KiCad: x nach rechts, y vom Antennen-Ende (0) zum USB-Ende (52),
// hier als -y abgebildet, damit die Draufsicht nicht gespiegelt ist. z = 0 ist die Unterseite der Trägerplatine.
// part = "base" | "lid" | "both" (Vorschau) | "print" (beide druckfertig nebeneinander)
part = "both";   // zusätzlich: "lid_text" = Schrift-Einlage für den Deckel (zweite Farbe)
$fn = 48;

// Version 1.1 (21.11.2026): Gehäuse 2 mm länger, dickere Wände um die Schrauben, größere Schrift als Einlage in Kontrastfarbe

// ---------- Platinen und Stapel ----------
W = 28.5;  L = 52.0;          // Trägerplatine = ESP32-Board
PCB = 1.6;                     // Trägerplatine 0 .. 1.6
ESP_BOT = 12.6;                // Buchsenleiste 8,5 + Stiftleisten-Kunststoff 2,5
ESP_TOP = ESP_BOT + 1.6;       // 14.2
HOLES = [[2.6, 2.6], [25.9, 2.6], [2.6, 49.4], [25.9, 49.4]];   // Trägerplatine v1.3 = ESP32-Löcher (Ø 3 mm, gemessen)

// ---------- Gehäuse ----------
c   = 0.5;     // Luft um die Platine
t   = 1.8;     // Wandstärke
tb  = 1.6;     // Boden
tt  = 2.0;     // Deckelplatte
zin_bot = -3.5;            // Innenboden (Lötstifte ragen ca. 3 mm nach unten)
zin_top = 18.4;            // Innendecke (ESP-Bauteile bis ca. 17,4)
S   = 9.0;                 // Trennebene
LIP = 2.5;  tl = 0.9;  fit = 0.15;   // Falz (nur zum Zentrieren)
R_out = 2.5;

// ---------- Verschraubung: 4 x M2 von unten ----------
// Schraubensäulen an den Ecken außerhalb der Platine, dazwischen eine Nische für die Stecker.
P      = 5.8;              // Säulentiefe über die Stirnwand hinaus (v1.1: +1 mm je Ende für dickere Wände um die Schrauben)
SA     = 2.2;              // Abstand Schraubenachse zur Stirnwand außen
NICHE  = [4.1, 24.4];      // x-Bereich der Steckernische an beiden Enden
SX     = [(-c - t + NICHE[0]) / 2, W - (-c - t + NICHE[0]) / 2];   // Schraubenachsen x, mittig zwischen Außenwand und Nische
SY     = [-c - t - SA, L + c + t + SA];   // Schraubenachsen y (KiCad)
SCREW_CLEAR = 2.4;         // Durchgang im Unterteil
HEAD_D = 4.2;  HEAD_H = 4.0;   // Senkung für den Zylinderkopf (DIN 912 / ISO 4762 M2: 3,8 x 2)
INSERT = false;            // true: Gewindeeinsatz M2 (3,2 x 4) statt selbstschneidend
PILOT  = INSERT ? 3.2 : 1.7;   PILOT_H = INSERT ? 4.6 : 8.0;
// Schraube: M2 x 16 (Unterteil 10,1 mm + ca. 6 mm Gewinde im Deckel)

zmin = zin_bot - tb;  zmax = zin_top + tt;

// ---------- Öffnungen (x0, x1, z0, z1) ----------
JST   = [[4.2, 11.0, 1.4, 6.8], [17.5, 24.3, 1.4, 6.8]];          // Antennen-Ende, y = 0
PIN_Z = PCB + 1.27;
SERVO_W = 8.0; SERVO_T = 2.5; GAP = 0.3;                          // Servostecker gemessen: 8 x 2,5 mm
SERVO = [for (cx = [9.16, 19.36]) [cx - SERVO_W / 2 - GAP, cx + SERVO_W / 2 + GAP, PIN_Z - SERVO_T / 2 - GAP, PIN_Z + SERVO_T / 2 + GAP]];   // USB-Ende, y = 52
USB   = [W / 2 - 6.0, W / 2 + 6.0, ESP_TOP + 1.6 - 3.5, ESP_TOP + 1.6 + 3.5];

module rr(x0, y0, x1, y1, r) {   // abgerundetes Rechteck in KiCad-Koordinaten
    translate([0, -y1]) offset(r = r) offset(delta = -r) translate([x0, 0]) square([x1 - x0, y1 - y0]);
}
module outer2d(g = 0) difference() {
    rr(-c - t - g, -c - t - P - g, W + c + t + g, L + c + t + P + g, R_out);
    for (yr = [[-c - t - P - 1, -c - t], [L + c + t, L + c + t + P + 1]])   // Steckernischen
        translate([NICHE[0], -yr[1]]) square([NICHE[1] - NICHE[0], yr[1] - yr[0]]);
}
module screws(d, z0, h) for (x = SX, y = SY) translate([x, -y, z0]) cylinder(d = d, h = h);
module inner2d(g = 0) rr(-c - g, -c - g, W + c + g, L + c + g, max(0.3, R_out - t));
module at(x, y) translate([x, -y, 0]) children();

module end_cut(o, yend) {  // Öffnung durch die Stirnwand bei y = 0 oder y = L
    y0 = yend == 0 ? -c - t - 1 : L;
    translate([o[0], -(y0 + c + t + 1), o[2]]) cube([o[1] - o[0], c + t + 1 + 0.01, o[3] - o[2]]);
}
module openings() {
    for (o = JST) end_cut(o, 0);
    for (o = SERVO) end_cut(o, L);
    end_cut([USB[0], USB[1], USB[2], USB[3]], L);
}

module shell() {
    difference() {
        translate([0, 0, zmin]) linear_extrude(zmax - zmin) outer2d();
        translate([0, 0, zin_bot]) linear_extrude(zin_top - zin_bot) inner2d();
    }
}


module base() {
    difference() {
        union() {
            intersection() { shell(); translate([-50, -100, zmin - 1]) cube([200, 200, S - zmin + 1]); }
            // Falz innen
            translate([0, 0, S - 0.01]) linear_extrude(LIP) difference() { inner2d(tl); inner2d(); }
            // Auflagen mit Zentrierstiften für die Montagelöcher
            for (h = HOLES) at(h[0], h[1]) {
                translate([0, 0, zin_bot - 0.01]) cylinder(d = 4.4, h = -zin_bot + 0.01);
                cylinder(d = 1.8, h = PCB - 0.2);
            }
        }
        openings();
        screws(SCREW_CLEAR, zmin - 1, S - zmin + 2);   // Durchgang
        screws(HEAD_D, zmin - 1, HEAD_H + 1);          // Kopfsenkung
    }
}

module lid() {
    difference() {
        union() {
            intersection() { shell(); translate([-50, -100, S]) cube([200, 200, zmax - S + 1]); }
            // Niederhalter: Zentrierstift ins ESP32-Loch (Ø 3), Schulter drückt auf den Lochrand
            for (h = HOLES) at(h[0], h[1]) {
                translate([0, 0, ESP_TOP + 0.05]) cylinder(d = 4.0, h = zin_top - ESP_TOP);
                translate([0, 0, ESP_TOP - 1.2]) cylinder(d = 2.6, h = 1.3);
            }
        }
        // Platz für den Falz des Unterteils
        translate([0, 0, S - 0.01]) linear_extrude(LIP + fit + 0.01) inner2d(tl + fit);
        screws(PILOT, S - 0.01, PILOT_H);              // Kernloch bzw. Einsatz
        openings();
        // Beschriftung als Einlage, TXT_D tief (2 Schichten à 0,2 mm)
        translate([0, 0, zmax - TXT_D]) linear_extrude(TXT_D + 1) labels();
    }
}

// ---------- Beschriftung ----------
TXT_D = 0.4;    // Tiefe der Schrift = 2 Schichten à 0,2 mm, in Kontrastfarbe drucken
FONT  = "Liberation Sans:style=Bold";
module labels() {
    translate([W / 2, -L / 2 + 3.5]) text("PB600", size = 6, font = FONT, halign = "center", valign = "center");
    translate([W / 2, -L / 2 - 4.5]) text("SOUND", size = 3.8, font = FONT, halign = "center", valign = "center");
    translate([7.4, -5.0]) text("7-18V", size = 3.4, font = FONT, halign = "center", valign = "center");
    translate([21.2, -5.0]) text("SPK", size = 3.4, font = FONT, halign = "center", valign = "center");
    translate([9.16, -L + 4.6]) text("THR", size = 3.4, font = FONT, halign = "center", valign = "center");
    translate([19.36, -L + 4.6]) text("FN", size = 3.4, font = FONT, halign = "center", valign = "center");
}
// Schrift-Einlage: füllt die Vertiefung im Deckel bündig aus
module lid_text() translate([0, 0, zmax - TXT_D]) linear_extrude(TXT_D) labels();

if (part == "base") translate([0, 0, -zmin]) base();
else if (part == "lid") translate([0, 0, zmax]) rotate([180, 0, 0]) translate([0, 0, 0]) lid();   // Deckel kopfüber drucken
else if (part == "lid_text") translate([0, 0, zmax]) rotate([180, 0, 0]) lid_text();             // gleiche Lage wie der Deckel
else if (part == "lid_text_asm") lid_text();
else if (part == "both") { base(); lid(); }
else if (part == "base_asm") base();
else if (part == "lid_asm") lid();
else if (part == "print") {
    translate([0, 0, -zmin]) base();
    translate([W * 2 + 12, 0, zmax]) rotate([0, 180, 0]) lid();
}
