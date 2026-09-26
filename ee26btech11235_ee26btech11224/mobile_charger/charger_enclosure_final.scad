// ============================================================================
// USB 5V CHARGER ENCLOSURE — FINAL PARAMETRIC CAD
// Built from the supplied master prompt, measurement status, CAD agent rules,
// validation protocol, reference photographs, and the final OpenSCAD-access
// chat correction regarding the perfboard-front USB/LED interface.
//
// IMPORTANT: This is an engineering CAD model, not a mains-safety certification.
// Unresolved physical dimensions are intentionally parameterized and listed in
// the accompanying validation report.
// ============================================================================

$fn = 48;

// -----------------------------
// OUTPUT CONTROL
// -----------------------------
PART = "assembly";
DEBUG = false;
SHOW_MEASURED_ENVELOPES = false;
DEBUG_UPPER_GAP = 8.0;
DEBUG_LID_GAP = 16.0;

// -----------------------------
// VERIFIED / SOURCE PARAMETERS
// -----------------------------
WALL = 2.8;                         // DESIGN BASELINE / MASTER PROMPT
BOARD_LENGTH = 70;                 // MASTER PROMPT
BOARD_WIDTH = 50;                  // MASTER PROMPT
BOARD_THICKNESS = 1.6;             // DESIGN ASSUMPTION — VERIFY ACTUAL BOARD
BOARD_FRONT_INSET = 4.0;           // DESIGN ASSUMPTION — front interface alignment
BOARD_LEFT_OFFSET = 8.2;           // FINAL CHAT CORRECTION / current placement
BOARD_HOLE_EDGE_OFFSET = 3.2;      // DESIGN ASSUMPTION — VERIFY hole grid
BOARD_HOLE_SLOT = 5.0;
BOARD_STANDOFF = 4.0;              // DESIGN BASELINE; preserves underside clearance

USB_REFERENCE_OFFSET = 12.5;       // USER MEASURED / FINAL CHAT CORRECTION
LED_REFERENCE_OFFSET = 39.0;       // USER MEASURED / FINAL CHAT CORRECTION
USB_FACE_WIDTH = 12.5;             // USER MEASURED approx.
USB_FACE_HEIGHT = 6.0;             // DESIGN ASSUMPTION from supplied measurement/photo
USB_OPEN_W = 13.8;                 // print clearance
USB_OPEN_H = 7.2;                  // print clearance
USB_BEZEL_W = 18.0;
USB_BEZEL_H = 11.0;
USB_BEZEL_DEPTH = 5.0;
USB_SUPPORT_CLEARANCE = 0.8;        // configurable until actual USB body depth verified
LED_DIAMETER = 3.9;                // USER MEASURED approx.
LED_HOLE_D = 4.5;
LED_BEZEL_OD = 7.0;
LED_BEZEL_ID = 4.5;
LED_BEZEL_DEPTH = 1.3;
INTERFACE_CENTER_ABOVE_BOARD = 4.0;// DESIGN ASSUMPTION — verify actual LED/USB height

TRANSFORMER_LENGTH = 78;           // USER/SOURCE NOMINAL CAD
TRANSFORMER_WIDTH = 50;
TRANSFORMER_HEIGHT = 50;
TRANSFORMER_WALL_CLEARANCE = 4.0;
TRANSFORMER_SUPPORT_H = 0.0;       // transformer rests on enclosure floor; retained by clamps
TRANSFORMER_TOP_CLEARANCE = 4.0;
TRANSFORMER_CLAMP_CLEARANCE = 1.0;
TRANSFORMER_CABLE_OD = 5.0;         // approx., VERIFY
TRANSFORMER_CABLE_CHANNEL_D = 6.4;  // configurable
TRANSFORMER_CABLE_CLAMP_HOLE_D = 5.8;
TRANSFORMER_CABLE_CHANNEL_DEPTH = 12;
TRANSFORMER_CABLE_ENTRY_Z = 22;
TRANSFORMER_CABLE_ENTRY_X = 14;     // DESIGN ASSUMPTION — VERIFY actual exit orientation
TRANSFORMER_MIN_BEND_R = 12;        // DESIGN ASSUMPTION — VERIFY

RESERVOIR_CAP_D = 12.6;              // USER MEASURED
RESERVOIR_CAP_H = 20.0;              // USER MEASURED
RESERVOIR_CAP_LEAD_SPACING = 5.0;    // USER MEASURED
OUTPUT_CAP_D = 8.0;                  // NEEDS PHYSICAL MEASUREMENT
OUTPUT_CAP_H = 12.0;                 // NEEDS PHYSICAL MEASUREMENT
BYPASS_CAP_D = 5.0;                  // DESIGN ASSUMPTION — small, verify if tight fit is needed
BYPASS_CAP_H = 6.0;                  // DESIGN ASSUMPTION

BUCK_BOARD_L = 43.18;                // NOMINAL — VERIFY actual module
BUCK_BOARD_W = 21.08;                // NOMINAL — VERIFY actual module
BUCK_MAX_H = 14.0;                   // NOMINAL — VERIFY maximum component height
BUCK_PCB_T = 1.6;                    // DESIGN ASSUMPTION — verify
BUCK_BOTTOM_CLEAR = 3.0;
BUCK_SIDE_CLEAR = 1.5;
BUCK_RAIL_H = 2.5;
BUCK_RAIL_W = 2.5;
BUCK_END_STOP = 1.5;
BUCK_POT_CLEARANCE = 3.0;

// -----------------------------
// ENCLOSURE / DERIVED GEOMETRY
// -----------------------------
GENERAL_CLEAR = 3.0;
TOP_CLEAR = 5.0;
MAT_TOL = 0.3;                       // configurable 0.2–0.4 mm baseline
LID_TOP_T = 2.8;
LID_LOCATOR_DEPTH = 5.5;
LID_LOCATOR_WALL = 2.0;

LOWER_H = WALL + TRANSFORMER_SUPPORT_H + TRANSFORMER_HEIGHT + TRANSFORMER_TOP_CLEARANCE;
UPPER_H = 49.5;
INNER_X = TRANSFORMER_LENGTH + 2*TRANSFORMER_WALL_CLEARANCE;
INNER_Y = TRANSFORMER_WIDTH + 2*TRANSFORMER_WALL_CLEARANCE;
OUT_X = INNER_X + 2*WALL;
OUT_Y = INNER_Y + 2*WALL;

// The divider is the floor of the removable upper electronics shell.  It is
// deliberately NOT part of the transformer tray so the transformer remains
// insertable from above before the upper shell is installed.
DIVIDER_T = 2.8;
BOARD_X = BOARD_LEFT_OFFSET;
BOARD_Y = BOARD_FRONT_INSET + WALL;
BOARD_Z = LOWER_H + DIVIDER_T + BOARD_STANDOFF;
BOARD_TOP_Z = BOARD_Z + BOARD_THICKNESS;
INTERFACE_Z = BOARD_TOP_Z + INTERFACE_CENTER_ABOVE_BOARD;
INTERFACE_Z_LOCAL = DIVIDER_T + BOARD_STANDOFF + BOARD_THICKNESS + INTERFACE_CENTER_ABOVE_BOARD;
USB_X = BOARD_X + USB_REFERENCE_OFFSET;
LED_X = BOARD_X + LED_REFERENCE_OFFSET;

// Buck module: 22 mm vertical gap above board top is the design assumption;
// the resulting 5.1 mm minimum lid clearance is asserted below.
BUCK_BOARD_Z = DIVIDER_T + BOARD_STANDOFF + BOARD_THICKNESS + 22.0;
BUCK_X = BOARD_LEFT_OFFSET + 13.5;   // PHOTO-BASED DESIGN ASSUMPTION

// Dedicated separate printable buck cradle.
BUCK_CRADLE_X = BUCK_BOARD_L + 2*BUCK_SIDE_CLEAR;
BUCK_CRADLE_Y = BUCK_BOARD_W + 2*BUCK_SIDE_CLEAR;
BUCK_CRADLE_X0 = BUCK_X - BUCK_SIDE_CLEAR;
BUCK_CRADLE_Y0 = (OUT_Y-WALL) - BUCK_CRADLE_Y;
BUCK_Y = BUCK_CRADLE_Y0 + BUCK_SIDE_CLEAR;
BUCK_CRADLE_RAIL_W = 2.5;
BUCK_CRADLE_RAIL_H = 2.5;
BUCK_CRADLE_LIP_H = 2.5;
BUCK_CRADLE_FRONT_STOP_H = 3.5;
BUCK_CRADLE_EAR_OUT = 5.0;
BUCK_CRADLE_EAR_DEPTH = 6.0;
BUCK_CRADLE_EAR_H = 5.0;
BUCK_CRADLE_EAR_Y_SPAN = 10.0;
BUCK_CRADLE_EAR_Y0 = BUCK_CRADLE_Y - BUCK_CRADLE_EAR_Y_SPAN + 2.0; // rear-mounted ears; keeps supports near the back wall
BUCK_CRADLE_SCREW_Z_LOCAL = 2.5;
BUCK_CRADLE_SCREW_D = 3.5; // M3 clearance hole
BUCK_CRADLE_SCREW_X_LOCAL = [-2.5, BUCK_CRADLE_X+2.5];
BUCK_CRADLE_SCREW_Y_LOCAL = BUCK_CRADLE_EAR_Y0 + BUCK_CRADLE_EAR_Y_SPAN/2; // nominally near rear wall
BUCK_CRADLE_SCREW_WALL_PAD_D = 6.0;
BUCK_CRADLE_SCREW_BOSS_H = 8.5;
BUCK_CRADLE_Z0 = BUCK_BOARD_Z - BUCK_CRADLE_RAIL_H;
BUCK_SUPPORT_Y = OUT_Y-WALL-3.0;       // rear support center; remains behind the perfboard back edge
BUCK_SUPPORT_D = 8.0;                  // robust printed support tower diameter
BUCK_SUPPORT_RIB = 3.0;                // rear-wall tie-in thickness

// Upper-to-lower locating lip; uses the lower tray inner wall as a datum.
UPPER_LOCATOR_DEPTH = 4.0;
UPPER_LOCATOR_WALL = 2.0;

// Low-voltage pass-through and guard, cut through the upper-shell divider floor.
LV_PASS_W = 14.0;
LV_PASS_H = 6.0;
LV_PASS_X = OUT_X - WALL - 20.0;
LV_PASS_Y = OUT_Y - WALL - 13.0;
LV_GUARD_H = 3.0;
LV_GUARD_WALL = 2.0;

// Board front retention keep-outs — FINAL OPENSCAD-ACCESS CHAT CORRECTION
USB_RAIL_KEEP = 22.0;
LED_RAIL_KEEP = 10.0;

// Lower ventilation
LOWER_VENT_D = 2.6;
LOWER_VENT_H = 14;
LOWER_VENT_SPACING = 7.5;
LOWER_VENT_COUNT = 3;

// Upper ventilation
UPPER_VENT_D = 2.8;
UPPER_VENT_H = 8;
TOP_VENT_W = 2.2;
TOP_VENT_L = 16;
TOP_VENT_COUNT = 5;

// Fasteners
M3_CLEAR = 3.5;
M3_PILOT = 2.7;
HEATSET_OD = 5.0;
HEATSET_DEPTH = 5.5;
CORNER_BOSS_D = 6.0;
CORNER_BOSS_R = CORNER_BOSS_D/2;
CORNER_BOSS_C = 3.0;
CORNER_PAD_SIZE = 6.0;

// Transformer clamps
TCLAMP_LEN_Y = 14;
TCLAMP_TAB_L = 7;
TCLAMP_T = 3;
TCLAMP_H = 8;
TCLAMP_SCREW_D = M3_CLEAR;
TCLAMP_Y1 = 11.0;
TCLAMP_Y2 = 43.0;

// Mains cable clamp
CABLE_CLAMP_W = 22;
CABLE_CLAMP_H = 14;
CABLE_CLAMP_T = 3.0;
CABLE_CLAMP_SCREW_SPAN = 16.0;

// -----------------------------
// ASSERTIONS / DESIGN-INVARIANT CHECKS
// -----------------------------
module validate_params(){
    assert(BOARD_LENGTH == 70, "BOARD_LENGTH must remain 70 mm");
    assert(BOARD_WIDTH == 50, "BOARD_WIDTH must remain 50 mm");
    assert(USB_X > BOARD_X && USB_X < BOARD_X + BOARD_LENGTH, "USB reference offset leaves board");
    assert(LED_X > BOARD_X && LED_X < BOARD_X + BOARD_LENGTH, "LED reference offset leaves board");
    assert(LED_X > USB_X, "LED must be to the right of USB on the front 70 mm edge");
    assert(OUT_X > TRANSFORMER_LENGTH + 2*WALL, "Transformer does not fit X envelope");
    assert(OUT_Y > TRANSFORMER_WIDTH + 2*WALL, "Transformer does not fit Y envelope");
    assert(TRANSFORMER_TOP_CLEARANCE >= 3, "Transformer top clearance is too small");
    assert(BOARD_STANDOFF >= 3, "Perfboard underside clearance below board is too small");
    assert(TOP_CLEAR >= 5, "Top clearance must remain >= 5 mm");
    assert(UPPER_H > (BUCK_BOARD_Z + BUCK_MAX_H + TOP_CLEAR), "Upper compartment too short for buck envelope");
    assert(UPPER_H > (BOARD_TOP_Z-LOWER_H) + RESERVOIR_CAP_H + TOP_CLEAR, "Upper compartment too short for reservoir capacitor");
    assert(USB_OPEN_W > USB_FACE_WIDTH, "USB opening must exceed measured face width");
    assert(USB_OPEN_H > USB_FACE_HEIGHT, "USB opening must exceed measured face height");
    assert(TRANSFORMER_CABLE_CHANNEL_D >= TRANSFORMER_CABLE_OD + 0.9, "Cable channel needs additional clearance");
    assert(BUCK_CRADLE_X > BUCK_BOARD_L && BUCK_CRADLE_Y > BUCK_BOARD_W, "Buck cradle must exceed PCB envelope");
    assert(BUCK_CRADLE_X0 >= WALL && BUCK_CRADLE_X0 + BUCK_CRADLE_X <= OUT_X-WALL+0.1, "Buck cradle X must stay inside upper compartment");
    assert(BUCK_CRADLE_Y0 >= WALL && BUCK_CRADLE_Y0 + BUCK_CRADLE_Y <= OUT_Y-WALL+0.1, "Buck cradle Y must stay inside upper compartment");
    assert(BUCK_SUPPORT_Y > BOARD_Y + BOARD_WIDTH, "Buck support towers must remain behind the perfboard back edge");
    assert(BUCK_SUPPORT_Y < OUT_Y-WALL, "Buck support towers must remain inside the back wall");
    assert(abs((BUCK_CRADLE_Y0 + BUCK_CRADLE_SCREW_Y_LOCAL) - BUCK_SUPPORT_Y) < 0.01, "Buck cradle rear screw holes must align with support towers");
    assert(OUT_X > BOARD_X + BOARD_LENGTH + 2, "Perfboard must fit with side clearance");
    echo("CAD INVARIANT CHECKS: PASS");
    echo("Board front interface: USB X =", USB_X, "LED X =", LED_X);
    echo("Overall enclosure nominal: ", OUT_X, " x ", OUT_Y, " x ", LOWER_H+UPPER_H+LID_TOP_T, " mm");
}

// -----------------------------
// BASIC GEOMETRY HELPERS
// -----------------------------
module chamfered_rect_2d(w,h,c){
    polygon([
        [-w/2+c,-h/2], [w/2-c,-h/2], [w/2,-h/2+c], [w/2,h/2-c],
        [w/2-c,h/2], [-w/2+c,h/2], [-w/2,h/2-c], [-w/2,-h/2+c]
    ]);
}

module chamfered_prism_y(w,h,depth,c=0.8){
    rotate([-90,0,0]) linear_extrude(height=depth) chamfered_rect_2d(w,h,c);
}

module slot_x(len, dia, depth){
    // capsule along X; axis of cylinders is X
    hull(){
        translate([-(len-dia)/2,0,0]) rotate([0,90,0]) cylinder(d=dia,h=depth);
        translate([(len-dia)/2,0,0]) rotate([0,90,0]) cylinder(d=dia,h=depth);
    }
}

module slot_y(len, dia, depth){
    hull(){
        translate([0,-(len-dia)/2,0]) rotate([90,0,0]) cylinder(d=dia,h=depth);
        translate([0,(len-dia)/2,0]) rotate([90,0,0]) cylinder(d=dia,h=depth);
    }
}

module slot_z_thru_x(len, dia, depth){
    // vertical capsule, extruded through an X-oriented side wall
    hull(){
        translate([0,0,-(len-dia)/2]) rotate([0,90,0]) cylinder(d=dia,h=depth);
        translate([0,0,(len-dia)/2]) rotate([0,90,0]) cylinder(d=dia,h=depth);
    }
}

module slot_y_thru_z(len, dia, depth){
    // capsule along Y, extruded vertically through a top/bottom surface
    hull(){
        translate([0,-(len-dia)/2,0]) cylinder(d=dia,h=depth);
        translate([0,(len-dia)/2,0]) cylinder(d=dia,h=depth);
    }
}

module boss_with_pilot(h=8){
    difference(){
        cylinder(d=CORNER_BOSS_D,h=h);
        translate([0,0,-0.1]) cylinder(d=M3_PILOT,h=h+0.2);
        translate([0,0,h-HEATSET_DEPTH]) cylinder(d=HEATSET_OD,h=HEATSET_DEPTH+0.15);
    }
}

module board_hole_slot(){
    // elongated screw clearance in the boss so exact board hole position can be
    // accommodated without inventing it.
    hull(){
        translate([-(BOARD_HOLE_SLOT-3.5)/2,0,0]) cylinder(d=3.5,h=6);
        translate([(BOARD_HOLE_SLOT-3.5)/2,0,0]) cylinder(d=3.5,h=6);
    }
}

// -----------------------------
// LOWER TRANSFORMER TRAY — OPEN TOP
// -----------------------------
module lower_base(){
    difference(){
        union(){
            // Open-top tray: only the floor and side walls remain.  The
            // divider is part of the upper shell so the transformer can be
            // installed and secured before the electronics shell is fitted.
            cube([OUT_X,OUT_Y,LOWER_H]);

            // reinforced corner pads for the long enclosure/cover screws
            for (xx=[0, OUT_X-CORNER_PAD_SIZE])
                for (yy=[0, OUT_Y-CORNER_PAD_SIZE])
                    translate([xx,yy,0]) cube([CORNER_PAD_SIZE,CORNER_PAD_SIZE,LOWER_H]);

            // transformer locating stops: four floor-connected low stops
            for (xx=[WALL+TRANSFORMER_WALL_CLEARANCE-6, OUT_X-WALL-TRANSFORMER_WALL_CLEARANCE])
                for (yy=[WALL+TRANSFORMER_WALL_CLEARANCE-6, OUT_Y-WALL-TRANSFORMER_WALL_CLEARANCE])
                    translate([xx,yy,WALL-0.3]) cube([6,6,8]);

            // transformer side clamp bosses
            for (yy=[TCLAMP_Y1, TCLAMP_Y2]){
                translate([-3.0,yy,WALL+TRANSFORMER_HEIGHT-3.5]) cube([3.3,14,8]);
                translate([OUT_X-0.3,yy,WALL+TRANSFORMER_HEIGHT-3.5]) cube([3.3,14,8]);
            }

            // mains cable clamp screw bosses on the rear exterior
            for (xx=[TRANSFORMER_CABLE_ENTRY_X-CABLE_CLAMP_SCREW_SPAN/2, TRANSFORMER_CABLE_ENTRY_X+CABLE_CLAMP_SCREW_SPAN/2])
                translate([xx-3,OUT_Y-0.3,WALL+TRANSFORMER_CABLE_ENTRY_Z-CABLE_CLAMP_H/2])
                    cube([6,3,CABLE_CLAMP_H]);
        }

        // inner transformer cavity — open all the way through the top
        translate([WALL,WALL,WALL])
            cube([INNER_X,INNER_Y,LOWER_H-WALL+0.2]);

        // lower-side ventilation slots
        for (side=[0,1])
            for (i=[0:LOWER_VENT_COUNT-1]){
                yy = 11 + i*LOWER_VENT_SPACING;
                if (side==0)
                    translate([-1,yy,22]) slot_z_thru_x(LOWER_VENT_H,LOWER_VENT_D,WALL+2);
                else
                    translate([OUT_X+1,yy,22]) mirror([1,0,0]) slot_z_thru_x(LOWER_VENT_H,LOWER_VENT_D,WALL+2);
            }

        // split cable-entry U-channel at the rear edge
        translate([TRANSFORMER_CABLE_ENTRY_X,OUT_Y-TRANSFORMER_CABLE_CHANNEL_DEPTH, TRANSFORMER_CABLE_ENTRY_Z])
            rotate([90,0,0]) cylinder(d=TRANSFORMER_CABLE_CHANNEL_D,h=TRANSFORMER_CABLE_CHANNEL_DEPTH+2);
        translate([TRANSFORMER_CABLE_ENTRY_X-TRANSFORMER_CABLE_CHANNEL_D/2,OUT_Y-TRANSFORMER_CABLE_CHANNEL_DEPTH-1,TRANSFORMER_CABLE_ENTRY_Z-TRANSFORMER_CABLE_CHANNEL_D/2])
            cube([TRANSFORMER_CABLE_CHANNEL_D,TRANSFORMER_CABLE_CHANNEL_DEPTH+2,TRANSFORMER_CABLE_CHANNEL_D]);

        // transformer clamp screw pilots
        for (yy=[TCLAMP_Y1,TCLAMP_Y2]){
            translate([-3.1,yy+TCLAMP_LEN_Y/2,WALL+TRANSFORMER_HEIGHT+2.5]) rotate([0,90,0]) cylinder(d=M3_PILOT,h=4);
            translate([OUT_X-0.1,yy+TCLAMP_LEN_Y/2,WALL+TRANSFORMER_HEIGHT+2.5]) rotate([0,90,0]) cylinder(d=M3_PILOT,h=4);
        }

        // cable clamp pilot holes
        for (xx=[TRANSFORMER_CABLE_ENTRY_X-CABLE_CLAMP_SCREW_SPAN/2, TRANSFORMER_CABLE_ENTRY_X+CABLE_CLAMP_SCREW_SPAN/2])
            translate([xx,OUT_Y-1,WALL+TRANSFORMER_CABLE_ENTRY_Z]) rotate([-90,0,0]) cylinder(d=M3_PILOT,h=5);

        // Four long M3 enclosure screw heat-set cavities in the lower tray.
        for (xx=[CORNER_BOSS_C,OUT_X-CORNER_BOSS_C])
            for (yy=[CORNER_BOSS_C,OUT_Y-CORNER_BOSS_C]){
                translate([xx,yy,LOWER_H-HEATSET_DEPTH]) cylinder(d=HEATSET_OD,h=HEATSET_DEPTH+0.15);
            }
    }
}

module front_rail_segment(x1,x2){
    if (x2>x1)
        translate([x1,BOARD_Y-0.5,DIVIDER_T-0.3])
            cube([x2-x1,3.0,(BOARD_Z-(LOWER_H))+1.0+0.3]);
}

module lv_guard(){
    // U-shaped low-voltage guide located immediately above the divider floor.
    translate([LV_PASS_X-2,LV_PASS_Y-2,DIVIDER_T-0.3]) cube([2,LV_PASS_H+4,LV_GUARD_H]);
    translate([LV_PASS_X+LV_PASS_W,LV_PASS_Y-2,DIVIDER_T-0.3]) cube([2,LV_PASS_H+4,LV_GUARD_H]);
    translate([LV_PASS_X,LV_PASS_Y-2,DIVIDER_T-0.3]) cube([LV_PASS_W,2,LV_GUARD_H]);
}

// -----------------------------
// UPPER ELECTRONICS SHELL — WITH INTEGRATED DIVIDER FLOOR
// -----------------------------
module upper_shell(){
    difference(){
        union(){
            // Full outer shell; cavity subtraction starts above the divider floor,
            // leaving DIVIDER_T mm of solid floor between transformer and electronics.
            cube([OUT_X,OUT_Y,UPPER_H]);
            for (xx=[0, OUT_X-CORNER_PAD_SIZE])
                for (yy=[0, OUT_Y-CORNER_PAD_SIZE])
                    translate([xx,yy,0]) cube([CORNER_PAD_SIZE,CORNER_PAD_SIZE,UPPER_H]);

            // Rear wall tie-in pads for the separate buck-cradle supports are added
            // after the main cavity subtraction below so they remain solid inside the cavity.

            // USB support collar — transfers cable insertion load into the enclosure.
            translate([USB_X, WALL-0.3, INTERFACE_Z_LOCAL])
                difference(){
                    chamfered_prism_y(USB_BEZEL_W,USB_BEZEL_H,USB_BEZEL_DEPTH,1.0);
                    translate([0,-0.2,0]) chamfered_prism_y(USB_OPEN_W+2*USB_SUPPORT_CLEARANCE,USB_OPEN_H+2*USB_SUPPORT_CLEARANCE,USB_BEZEL_DEPTH+0.4,0.7);
                }

            // LV guard above divider floor.
            lv_guard();

            // board locating rails — front rail deliberately interrupted for USB/LED
            translate([BOARD_X-1.5,BOARD_Y,DIVIDER_T-0.3]) cube([2.0,BOARD_WIDTH,(BOARD_STANDOFF+BOARD_THICKNESS)+1.0]);
            translate([BOARD_X+BOARD_LENGTH-0.5,BOARD_Y,DIVIDER_T-0.3]) cube([2.0,BOARD_WIDTH,(BOARD_STANDOFF+BOARD_THICKNESS)+1.0]);
            translate([BOARD_X,BOARD_Y+BOARD_WIDTH-1.5,DIVIDER_T-0.3]) cube([BOARD_LENGTH,2.0,(BOARD_STANDOFF+BOARD_THICKNESS)+1.0]);
            front_rail_segment(BOARD_X+0.8, USB_X-USB_RAIL_KEEP/2-0.8);
            front_rail_segment(USB_X+USB_RAIL_KEEP/2+0.8, LED_X-LED_RAIL_KEEP/2-0.8);
            front_rail_segment(LED_X+LED_RAIL_KEEP/2+0.8, BOARD_X+BOARD_LENGTH-0.8);

            // four perfboard standoffs, mechanically independent of enclosure walls
            for (sx=[BOARD_X+BOARD_HOLE_EDGE_OFFSET, BOARD_X+BOARD_LENGTH-BOARD_HOLE_EDGE_OFFSET])
                for (sy=[BOARD_Y+BOARD_HOLE_EDGE_OFFSET, BOARD_Y+BOARD_WIDTH-BOARD_HOLE_EDGE_OFFSET])
                    translate([sx,sy,DIVIDER_T-0.3]) cylinder(d=7,h=BOARD_STANDOFF+BOARD_THICKNESS+0.3);

            // small alignment tabs around the upper shell's bottom edge
            translate([10,WALL-1.0,0]) cube([12,2.0,1.5]);
            translate([OUT_X-22,WALL-1.0,0]) cube([12,2.0,1.5]);
            translate([10,OUT_Y-WALL-1.0,0]) cube([12,2.0,1.5]);
            translate([OUT_X-22,OUT_Y-WALL-1.0,0]) cube([12,2.0,1.5]);
        }

        // main electronics cavity — starts above divider floor
        translate([WALL,WALL,DIVIDER_T])
            cube([INNER_X,INNER_Y,UPPER_H-DIVIDER_T+0.2]);

        // divider pass-through for transformer secondary / low-voltage wiring
        translate([LV_PASS_X,LV_PASS_Y,-0.1]) cube([LV_PASS_W,LV_PASS_H,DIVIDER_T+0.2]);

        // USB aperture
        translate([USB_X,-1,INTERFACE_Z_LOCAL]) chamfered_prism_y(USB_OPEN_W,USB_OPEN_H,WALL+2,0.8);
        // LED aperture and shallow front bezel
        translate([LED_X,-1,INTERFACE_Z_LOCAL]) rotate([-90,0,0]) cylinder(d=LED_HOLE_D,h=WALL+2);
        translate([LED_X,-LED_BEZEL_DEPTH,INTERFACE_Z_LOCAL]) rotate([-90,0,0]) cylinder(d=LED_BEZEL_OD,h=LED_BEZEL_DEPTH+0.05);

        // upper side ventilation — low-voltage section only
        for (side=[0,1])
            for (i=[0:2]){
                zz = 17 + i*11;
                yy = 20 + (i%2)*20;
                if (side==0)
                    translate([-1,yy,zz]) slot_z_thru_x(UPPER_VENT_H,UPPER_VENT_D,WALL+2);
                else
                    translate([OUT_X+1,yy,zz]) mirror([1,0,0]) slot_z_thru_x(UPPER_VENT_H,UPPER_VENT_D,WALL+2);
            }

        // cover screw clearance holes
        for (xx=[CORNER_BOSS_C,OUT_X-CORNER_BOSS_C])
            for (yy=[CORNER_BOSS_C,OUT_Y-CORNER_BOSS_C])
                translate([xx,yy,-0.1]) cylinder(d=M3_CLEAR,h=UPPER_H+0.2);
    }

    // Internal locating lip that enters the lower transformer tray by 4 mm.
    // It is added after the main cavity so it remains an intentional solid feature.
    translate([WALL+MAT_TOL,WALL+MAT_TOL,-UPPER_LOCATOR_DEPTH])
        cube([INNER_X-2*MAT_TOL,UPPER_LOCATOR_WALL,UPPER_LOCATOR_DEPTH]);
    translate([WALL+MAT_TOL,OUT_Y-WALL-MAT_TOL-UPPER_LOCATOR_WALL,-UPPER_LOCATOR_DEPTH])
        cube([INNER_X-2*MAT_TOL,UPPER_LOCATOR_WALL,UPPER_LOCATOR_DEPTH]);
    translate([WALL+MAT_TOL,WALL+MAT_TOL,-UPPER_LOCATOR_DEPTH])
        cube([UPPER_LOCATOR_WALL,INNER_Y-2*MAT_TOL,UPPER_LOCATOR_DEPTH]);
    translate([OUT_X-WALL-MAT_TOL-UPPER_LOCATOR_WALL,WALL+MAT_TOL,-UPPER_LOCATOR_DEPTH])
        cube([UPPER_LOCATOR_WALL,INNER_Y-2*MAT_TOL,UPPER_LOCATOR_DEPTH]);

    // Buck-cradle support towers: rise from the divider to the actual cradle base.
    // They are placed at the rear edge so the perfboard remains free to seat.
    for (xx=[BUCK_CRADLE_X0+BUCK_CRADLE_SCREW_X_LOCAL[0], BUCK_CRADLE_X0+BUCK_CRADLE_SCREW_X_LOCAL[1]]){
        difference(){
            union(){
                translate([xx,BUCK_SUPPORT_Y,DIVIDER_T-0.3])
                    cylinder(d=BUCK_SUPPORT_D,h=BUCK_CRADLE_Z0-(DIVIDER_T-0.3));
                // tie the tower into the rear wall for bending stiffness
                translate([xx-BUCK_SUPPORT_D/2,BUCK_SUPPORT_Y,DIVIDER_T-0.3])
                    cube([BUCK_SUPPORT_D,OUT_Y-WALL-BUCK_SUPPORT_Y,BUCK_CRADLE_Z0-(DIVIDER_T-0.3)]);
            }
            translate([xx,BUCK_SUPPORT_Y,DIVIDER_T-0.4])
                cylinder(d=M3_PILOT,h=BUCK_CRADLE_Z0-(DIVIDER_T-0.3)+0.8);
            translate([xx,BUCK_SUPPORT_Y,BUCK_CRADLE_Z0-HEATSET_DEPTH+0.1])
                cylinder(d=HEATSET_OD,h=HEATSET_DEPTH+0.15);
        }
    }
}

module buck_cradle(){
    // Separate printable tray. Long side rails carry the PCB; the centre remains
    // open for underside solder joints and wiring. Two rear ears accept M3 screws into
    // heat-set inserts in the upper-shell support towers. The board drops in from above.
    difference(){
        union(){
            // support rails
            cube([BUCK_CRADLE_X,BUCK_CRADLE_RAIL_W,BUCK_CRADLE_RAIL_H]);
            translate([0,BUCK_CRADLE_Y-BUCK_CRADLE_RAIL_W,0])
                cube([BUCK_CRADLE_X,BUCK_CRADLE_RAIL_W,BUCK_CRADLE_RAIL_H]);

            // lateral retaining lips
            translate([0,0,0]) cube([BUCK_CRADLE_X,BUCK_CRADLE_RAIL_W,BUCK_CRADLE_RAIL_H+BUCK_CRADLE_LIP_H]);
            translate([0,BUCK_CRADLE_Y-BUCK_CRADLE_RAIL_W,0]) cube([BUCK_CRADLE_X,BUCK_CRADLE_RAIL_W,BUCK_CRADLE_RAIL_H+BUCK_CRADLE_LIP_H]);

            // front retention stop; board drops in from above
            cube([1.5,BUCK_CRADLE_Y,BUCK_CRADLE_FRONT_STOP_H]);

            // two outer rear mounting ears (outside the PCB envelope; aligned to upper-shell support towers)
            translate([-BUCK_CRADLE_EAR_OUT,BUCK_CRADLE_EAR_Y0,0])
                cube([BUCK_CRADLE_EAR_OUT+1.5,BUCK_CRADLE_EAR_Y_SPAN,BUCK_CRADLE_EAR_H]);
            translate([BUCK_CRADLE_X-1.5,BUCK_CRADLE_EAR_Y0,0])
                cube([BUCK_CRADLE_EAR_OUT+1.5,BUCK_CRADLE_EAR_Y_SPAN,BUCK_CRADLE_EAR_H]);
        }
        // vertical M3 clearance holes through the two ears
        for (xx=BUCK_CRADLE_SCREW_X_LOCAL)
            translate([xx,BUCK_CRADLE_SCREW_Y_LOCAL,-0.1])
                cylinder(d=BUCK_CRADLE_SCREW_D,h=BUCK_CRADLE_EAR_H+0.2);
    }
}

// -----------------------------
// LID
// -----------------------------
module lid(){
    difference(){
        union(){
            cube([OUT_X,OUT_Y,LID_TOP_T]);
            // internal locating lip
            translate([WALL+MAT_TOL,WALL+MAT_TOL,-LID_LOCATOR_DEPTH])
                difference(){
                    cube([INNER_X-2*MAT_TOL,INNER_Y-2*MAT_TOL,LID_LOCATOR_DEPTH]);
                    translate([LID_LOCATOR_WALL,LID_LOCATOR_WALL,-0.1])
                        cube([INNER_X-2*MAT_TOL-2*LID_LOCATOR_WALL,INNER_Y-2*MAT_TOL-2*LID_LOCATOR_WALL,LID_LOCATOR_DEPTH+0.2]);
                }
        }
        // cover screw clearance holes
        for (xx=[CORNER_BOSS_C,OUT_X-CORNER_BOSS_C])
            for (yy=[CORNER_BOSS_C,OUT_Y-CORNER_BOSS_C])
                translate([xx,yy,-0.1]) cylinder(d=M3_CLEAR,h=LID_TOP_T+0.2);

        // top exhaust over buck / low-voltage region
        for (i=[0:TOP_VENT_COUNT-1]){
            xx = BUCK_X + 3 + i*(BUCK_BOARD_L-8)/4;
            translate([xx,BUCK_Y+2,-0.1]) slot_y_thru_z(TOP_VENT_L,TOP_VENT_W,LID_TOP_T+0.2);
        }
    }
}

// -----------------------------
// TRANSFORMER CLAMPS — PRINT 2 OF EACH
// -----------------------------
module transformer_clamp(){
    // Universal side clamp. Print four copies; rotate two copies 180° in the XY plane
    // for the opposite side. The clamp itself is simple and support-free to print.
    difference(){
        union(){
            cube([TCLAMP_T,TCLAMP_LEN_Y,TCLAMP_H]);
            translate([0,0,TCLAMP_H-TCLAMP_T]) cube([TCLAMP_TAB_L,TCLAMP_LEN_Y,TCLAMP_T]);
        }
        translate([-0.1,TCLAMP_LEN_Y/2,TCLAMP_H-TCLAMP_T/2]) rotate([0,90,0]) cylinder(d=TCLAMP_SCREW_D,h=TCLAMP_T+0.2);
    }
}

module transformer_clamp_left(){ transformer_clamp(); }
module transformer_clamp_right(){ mirror([1,0,0]) translate([-TCLAMP_TAB_L,0,0]) transformer_clamp(); }

// -----------------------------
// MAINS CABLE CLAMP
// -----------------------------
module cable_clamp(){
    difference(){
        union(){
            cube([CABLE_CLAMP_W,CABLE_CLAMP_T,CABLE_CLAMP_H]);
            // small locating ears
            translate([-2,0,1]) cube([2,3,12]);
            translate([CABLE_CLAMP_W,0,1]) cube([2,3,12]);
        }
        // cable grip hole (intentionally tight, configurable)
        translate([CABLE_CLAMP_W/2,-0.2,CABLE_CLAMP_H/2]) rotate([-90,0,0]) cylinder(d=TRANSFORMER_CABLE_CLAMP_HOLE_D,h=CABLE_CLAMP_T+0.4);
        // horizontal screw holes in ears / bosses
        for (xx=[CABLE_CLAMP_W/2-CABLE_CLAMP_SCREW_SPAN/2, CABLE_CLAMP_W/2+CABLE_CLAMP_SCREW_SPAN/2])
            translate([xx,-0.2,CABLE_CLAMP_H/2]) rotate([-90,0,0]) cylinder(d=M3_CLEAR,h=CABLE_CLAMP_T+0.4);
    }
}

// -----------------------------
// DEBUG / VALIDATION VISUALIZATION
// -----------------------------
module debug_model(){
    // Exploded vertical view: components are moved with the upper shell so the
    // debug image tests the actual seating relationships rather than showing
    // the electronics in an unrelated coordinate frame.
    color([0.7,0.7,0.7,0.35]) translate([0,0,0]) lower_base();
    color([0.3,0.6,1.0,0.25]) translate([0,0,LOWER_H+DEBUG_UPPER_GAP]) upper_shell();
    color([0.9,0.9,0.9,0.35]) translate([0,0,LOWER_H+UPPER_H+DEBUG_LID_GAP]) lid();

    // Actual-sized upper electronics envelopes, shifted together with the exploded shell.
    color([0.1,0.7,0.2,0.35])
        translate([BOARD_X,BOARD_Y,BOARD_Z+DEBUG_UPPER_GAP]) cube([BOARD_LENGTH,BOARD_WIDTH,BOARD_THICKNESS]);
    color([1,0.65,0,0.65])
        translate([USB_X-USB_FACE_WIDTH/2,BOARD_Y-USB_FACE_HEIGHT,INTERFACE_Z-USB_FACE_HEIGHT/2+DEBUG_UPPER_GAP]) cube([USB_FACE_WIDTH,USB_FACE_HEIGHT,USB_FACE_HEIGHT]);
    color([1,0,0,0.65])
        translate([LED_X,0,INTERFACE_Z+DEBUG_UPPER_GAP]) rotate([90,0,0]) cylinder(d=LED_DIAMETER,h=10);
    cap_x = BOARD_X + 17.0;
    cap_y = BOARD_Y + 10.0;
    color([0.1,0.2,1,0.55])
        translate([cap_x,cap_y,BOARD_TOP_Z+DEBUG_UPPER_GAP]) cylinder(d=RESERVOIR_CAP_D,h=RESERVOIR_CAP_H);
    color([0.25,0.1,0.1,0.6])
        translate([BUCK_X,BUCK_Y,LOWER_H+BUCK_BOARD_Z+DEBUG_UPPER_GAP]) cube([BUCK_BOARD_L,BUCK_BOARD_W,BUCK_MAX_H]);
    color([0.15,0.15,0.15,0.45])
        translate([BUCK_CRADLE_X0,BUCK_CRADLE_Y0,LOWER_H+BUCK_CRADLE_Z0+DEBUG_UPPER_GAP]) buck_cradle();
}


module preview_model(){
    // Presentation / dimensional preview: enclosure plus transparent component envelopes.
    // Envelopes are deliberately labeled in accompanying documentation; they are NOT
    // claims about unmeasured exact geometry.
    color([0.65,0.65,0.68,0.35]) lower_base();
    color([0.45,0.6,0.95,0.28]) translate([0,0,LOWER_H]) upper_shell();
    color([0.8,0.8,0.82,0.35]) translate([0,0,LOWER_H+UPPER_H]) lid();

    // transformer envelope
    color([0.4,0.25,0.15,0.55])
        translate([WALL+TRANSFORMER_WALL_CLEARANCE,WALL+TRANSFORMER_WALL_CLEARANCE,WALL])
            cube([TRANSFORMER_LENGTH,TRANSFORMER_WIDTH,TRANSFORMER_HEIGHT]);

    // perfboard envelope + measured/estimated interface components
    color([0.15,0.6,0.18,0.65])
        translate([BOARD_X,BOARD_Y,BOARD_Z]) cube([BOARD_LENGTH,BOARD_WIDTH,BOARD_THICKNESS]);
    color([0.95,0.55,0.1,0.8])
        translate([USB_X-USB_FACE_WIDTH/2,BOARD_Y-USB_FACE_HEIGHT/2,INTERFACE_Z-USB_FACE_HEIGHT/2])
            cube([USB_FACE_WIDTH,USB_FACE_HEIGHT,USB_FACE_HEIGHT]);
    color([1,0.15,0.15,0.9])
        translate([LED_X,BOARD_Y-4,INTERFACE_Z]) rotate([-90,0,0]) cylinder(d=LED_DIAMETER,h=10);

    // reservoir envelope at photo-based position for spatial review only
    cap_x = BOARD_X + 17.0;
    cap_y = BOARD_Y + 10.0;
    color([0.05,0.2,0.9,0.65])
        translate([cap_x,cap_y,BOARD_TOP_Z]) cylinder(d=RESERVOIR_CAP_D,h=RESERVOIR_CAP_H);

    // buck module envelope and W103 trimpot clearance zone
    color([0.1,0.1,0.1,0.75])
        translate([BUCK_X,BUCK_Y,LOWER_H+BUCK_BOARD_Z]) cube([BUCK_BOARD_L,BUCK_BOARD_W,BUCK_MAX_H]);
    color([0.15,0.15,0.15,0.45])
        translate([BUCK_CRADLE_X0,BUCK_CRADLE_Y0,LOWER_H+BUCK_CRADLE_Z0]) buck_cradle();

    // secondary wire corridor visualization
    color([1.0,0.4,0.05,0.35])
        translate([LV_PASS_X,LV_PASS_Y,LOWER_H-0.2]) cube([LV_PASS_W,LV_PASS_H,6]);
}

// assembly for preview
module assembly(){
    lower_base();
    translate([0,0,LOWER_H]) upper_shell();
    translate([BUCK_CRADLE_X0,BUCK_CRADLE_Y0,LOWER_H+BUCK_CRADLE_Z0]) buck_cradle();
    translate([0,0,LOWER_H+UPPER_H]) lid();
}

// section view helper
module section_view(){
    // Section through the shell plus the same transparent component envelopes
    // used by the presentation preview.
    difference(){
        preview_model();
        translate([-1,-1,-1]) cube([OUT_X/2+1,OUT_Y+2,LOWER_H+UPPER_H+LID_TOP_T+2]);
    }
}

// -----------------------------
// MAIN DISPATCH
// -----------------------------
validate_params();

if (PART=="base") lower_base();
else if (PART=="upper") upper_shell();
else if (PART=="lid") lid();
else if (PART=="transformer_clamp") transformer_clamp();
else if (PART=="transformer_clamp_left") transformer_clamp_left();
else if (PART=="transformer_clamp_right") transformer_clamp_right();
else if (PART=="cable_clamp") cable_clamp();
else if (PART=="buck_cradle") buck_cradle();
else if (PART=="debug") debug_model();
else if (PART=="preview") preview_model();
else if (PART=="section") section_view();
else assembly();
