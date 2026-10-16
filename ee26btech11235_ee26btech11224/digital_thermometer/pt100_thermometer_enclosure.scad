/*
 Arduino PT100 Thermometer Enclosure — final submission prototype
 Units: millimetres. Parametric OpenSCAD source.
 Retained design requirements:
 - Potentiometer shaft/extension exits LEFT VERTICAL SIDE WALL, never through the floor.
 - PT100 cable exits RIGHT VERTICAL SIDE WALL.
 - Single Celsius/Fahrenheit plunger for D8 plus one retaining C-clip.
 - Perfboard supported level on four coplanar pads.
 - Raised LCD platform rails with ten posts support the overhanging LCD independently.
 - LCD display tunnel, M3 screws/bosses, locating lip, and USB-B opening.
 - USB opening's nearer end is 16.2 mm from the Arduino's right edge; opening is on the short (65.8 mm) end wall, with width along the 77.8 mm board direction as described by user.
 Important fit limits: pot axis and barrel length, USB height/connector shape, LCD aperture, sensor route, button travel, all solder/component heights require physical dry-fit. This model is not a certified electrical enclosure.
*/

part = "assembly"; // [assembly, base, lid, button_plunger, button_retaining_clip, pot_shaft_extension]
$fn = 64;
show_components = true;

// -------------------- Main enclosure --------------------
case_x = 114.65;
case_y = 169.0;
case_z = 38.20;
wall = 2.8;
floor_t = 2.8;
lid_t = 2.8;
base_h = 28.0;
lid_h = case_z - base_h;
lid_overlap = 3.0;
lid_clearance = 0.30;
lip_t = 1.8;
corner_r = 3.5;

// -------------------- Perfboard placement --------------------
pcb_x0 = 20.0;
pcb_y0 = 35.0;
pcb_z = 5.0;      // underside of perfboard above enclosure floor
pcb_w = 70.0;
pcb_l = 50.0;
pcb_t = 1.65;
pcb_pad_size = 5.0;
pcb_pad_inset = 2.0;
pcb_top_z = pcb_z + pcb_t;

// -------------------- LCD envelope and window --------------------
lcd_module_left_overhang = 1.5;
lcd_module_right_overhang = 8.2;
lcd_module_y_overhang = 30.0;
lcd_module_y_back = 6.0;
lcd_module_w = pcb_w + lcd_module_left_overhang + lcd_module_right_overhang;
lcd_module_d = lcd_module_y_overhang + lcd_module_y_back;
lcd_x0 = pcb_x0 - lcd_module_left_overhang;
lcd_y0 = pcb_y0 - lcd_module_y_overhang;
lcd_body_base_height = 19.6;
lcd_top_height = 23.1;
lcd_body_thickness = lcd_top_height - lcd_body_base_height;
lcd_body_bottom_z = pcb_z + lcd_body_base_height;
lcd_face_z = pcb_z + lcd_top_height;
lcd_aperture_w = 66.0;       // provisional: verify clear active glass/window
lcd_aperture_h = 16.0;       // provisional: verify clear active glass/window
lcd_window_offset_x = 0.0;
lcd_window_offset_y = 0.0;
lcd_window_x0 = lcd_x0 + (lcd_module_w - lcd_aperture_w)/2 + lcd_window_offset_x;
lcd_window_y0 = lcd_y0 + (lcd_module_d - lcd_aperture_h)/2 + lcd_window_offset_y;
lcd_bezel_wall = 2.0;
lcd_tunnel_clearance = 1.5;
lcd_tunnel_bottom_z = lcd_face_z + lcd_tunnel_clearance;

// -------------------- Raised LCD platform --------------------
lcd_support_enable = true;
lcd_support_top_z = lcd_body_bottom_z; // underside support height; dry-fit critical
lcd_support_rail_t = 3.0;
lcd_support_rail_w = 5.0;
lcd_support_post_size = 5.0;
lcd_support_front_y = lcd_y0 + 4.0;
lcd_support_rear_y = pcb_y0 - 7.0;
lcd_support_x0 = lcd_x0 + 1.5;
lcd_support_x1 = lcd_x0 + lcd_module_w - 1.5;
lcd_support_post_xs = [pcb_x0 + 6.5, pcb_x0 + 22.0,
                        (lcd_x0 + lcd_x0 + lcd_module_w)/2,
                        pcb_x0 + 54.0, pcb_x0 + pcb_w - 1.0];

// -------------------- Potentiometer side-wall access --------------------
pot_body_d = 16.3;
pot_shaft_d = 4.1;
pot_wall_hole_d = 7.0;
pot_extension_od = 6.4;
pot_extension_bore_d = 4.5;
pot_extension_external_len = 15.0;
pot_extension_collar_d = 8.0;
pot_extension_collar_len = 1.8;
pot_y = pcb_y0 - 12.0;       // provisional y=23 mm
pot_axis_z = 25.5;            // provisional horizontal centreline: measure real pot
pot_body_start_x = wall + 0.4;
pot_body_length = 12.0;       // provisional; change to measured barrel length

// -------------------- Celsius/Fahrenheit push button --------------------
button_d = 5.5;
button_height_above_pcb = 6.4;
button_pcb_x = 60.0;
button_pcb_y = 25.2;
button_x = pcb_x0 + button_pcb_x;
button_y = pcb_y0 + button_pcb_y;
button_hole_d = 4.8;
button_recess_d = 8.2;
button_recess_depth = 2.1;
button_top_z = pcb_z + button_height_above_pcb;
button_tip_gap = 0.25;
plunger_stem_d = 4.0;
plunger_head_d = 7.2;
plunger_head_h = 1.4;
plunger_head_recess = 0.15;
plunger_guide_od = 7.6;
plunger_guide_len = 1.4;
plunger_travel = 0.55;
plunger_clip_t = 1.4;
plunger_clip_id = 3.8;
plunger_clip_od = 8.0;
plunger_clip_gap = 1.6;
plunger_capture_clearance = 0.5;
plunger_stem_len = case_z - plunger_head_h - plunger_head_recess - (button_top_z + button_tip_gap);
plunger_clip_groove_z = case_z - lid_t - plunger_guide_len - plunger_capture_clearance
                        - plunger_clip_t/2 - (button_top_z + button_tip_gap);

// -------------------- PT100 cable exit --------------------
pt100_wire_d = 3.3;
pt100_exit_d = 6.0;
pt100_exit_y = pcb_y0 + 47.0;
pt100_exit_z = 12.0;           // adjust to the real wire route and bend radius

// -------------------- Arduino Uno envelope / USB opening --------------------
uno_w = 65.8;
uno_l = 77.8;
uno_z_envelope = 23.3;
uno_x0 = (case_x - uno_w)/2;
uno_y0 = pcb_y0 + pcb_l;
usb_opening_w = 13.0;
usb_opening_h = 12.0;
usb_near_edge_from_uno_right = 16.2;
// User measurement: nearer edge of opening is 16.2 mm from the Arduino right edge.
// Therefore the opening's right edge is uno_x0+uno_w-16.2.
usb_center_x = uno_x0 + uno_w - usb_near_edge_from_uno_right - usb_opening_w/2;
usb_center_z = pcb_z + 12.0;    // provisional: measure connector vertical centre

// -------------------- Fasteners --------------------
screw_d = 3.2;
screw_head_recess_d = 6.2;
screw_head_recess_depth = 1.2;
boss_d = 7.8;
insert_pocket_d = 4.6;          // confirm against specific heat-set insert
insert_pocket_depth = 7.5;
screw_xy = [[10.0,10.0],[case_x-10.0,10.0],
            [10.0,case_y-10.0],[case_x-10.0,case_y-10.0]];

// -------------------- Helpers --------------------
module rounded_prism_xy(x,y,h,r) {
  if (r <= 0) cube([x,y,h]);
  else hull() for (xx=[r,x-r]) for (yy=[r,y-r])
    translate([xx,yy,0]) cylinder(h=h,r=r);
}

module screw_bosses() {
  for (p=screw_xy) difference() {
    translate([p[0],p[1],floor_t]) cylinder(h=base_h-floor_t,d=boss_d);
    translate([p[0],p[1],base_h-insert_pocket_depth])
      cylinder(h=insert_pocket_depth+0.2,d=insert_pocket_d);
  }
}

module perfboard_supports() {
  for (x=[pcb_x0+pcb_pad_inset, pcb_x0+pcb_w-pcb_pad_inset-pcb_pad_size])
    for (y=[pcb_y0+pcb_pad_inset, pcb_y0+pcb_l-pcb_pad_inset-pcb_pad_size])
      translate([x,y,floor_t]) cube([pcb_pad_size,pcb_pad_size,pcb_z-floor_t]);
}

module raised_lcd_platform() {
  if (lcd_support_enable) {
    rail_bottom = lcd_support_top_z-lcd_support_rail_t;
    for (yy=[lcd_support_front_y,lcd_support_rear_y]) {
      translate([lcd_support_x0,yy,rail_bottom])
        cube([lcd_support_x1-lcd_support_x0,lcd_support_rail_w,lcd_support_rail_t]);
      for (xx=lcd_support_post_xs)
        translate([xx-lcd_support_post_size/2,
                   yy+lcd_support_rail_w/2-lcd_support_post_size/2,floor_t])
          cube([lcd_support_post_size,lcd_support_post_size,rail_bottom-floor_t]);
    }
  }
}

module base_shell() {
  difference() {
    rounded_prism_xy(case_x,case_y,base_h,corner_r);
    translate([wall,wall,floor_t])
      rounded_prism_xy(case_x-2*wall,case_y-2*wall,base_h-floor_t+0.02,max(0.4,corner_r-wall));
    // Left vertical wall: horizontal potentiometer shaft.
    translate([-1,pot_y,pot_axis_z]) rotate([0,90,0]) cylinder(h=wall+2,d=pot_wall_hole_d);
    // Right vertical wall: PT100 cable outlet.
    translate([case_x-wall-1,pt100_exit_y,pt100_exit_z]) rotate([0,90,0]) cylinder(h=wall+2,d=pt100_exit_d);
    // Rear (+Y) vertical wall: USB opening. Width coordinate follows the 65.8 mm end edge.
    translate([usb_center_x-usb_opening_w/2,case_y-wall-1,usb_center_z-usb_opening_h/2])
      cube([usb_opening_w,wall+2,usb_opening_h]);
  }
}

module base_tray() {
  union() { base_shell(); screw_bosses(); perfboard_supports(); raised_lcd_platform(); }
}

module lid_shell() {
  difference() {
    union() {
      rounded_prism_xy(case_x,case_y,lid_h,corner_r);
      // Locating lip, 3 mm deep, with 0.30 mm nominal clearance per side.
      difference() {
        translate([wall+lid_clearance,wall+lid_clearance,-lid_overlap])
          cube([case_x-2*(wall+lid_clearance),case_y-2*(wall+lid_clearance),lid_overlap]);
        translate([wall+lid_clearance+lip_t,wall+lid_clearance+lip_t,-lid_overlap-0.1])
          cube([case_x-2*(wall+lid_clearance+lip_t),case_y-2*(wall+lid_clearance+lip_t),lid_overlap+0.2]);
      }
      // Webs bridge lip to walls, positioned away from corner bosses.
      for (yy=[42.0,case_y-42.0]) translate([wall-0.01,yy-1.0,-lid_overlap]) cube([lid_clearance+0.02,2.0,lid_overlap]);
      for (xx=[26.0,case_x-26.0]) translate([xx-1.0,wall-0.01,-lid_overlap]) cube([2.0,lid_clearance+0.02,lid_overlap]);
    }
    // Hollow lid from underside, preserving 2.8 mm roof.
    translate([wall,wall,0]) rounded_prism_xy(case_x-2*wall,case_y-2*wall,lid_h-lid_t+0.01,max(0.4,corner_r-wall));
    // Screw holes and head recesses.
    for (p=screw_xy) {
      translate([p[0],p[1],lid_h-lid_t-0.1]) cylinder(h=lid_t+0.3,d=screw_d);
      translate([p[0],p[1],lid_h-screw_head_recess_depth]) cylinder(h=screw_head_recess_depth+0.2,d=screw_head_recess_d);
    }
    // LCD aperture.
    translate([lcd_window_x0,lcd_window_y0,lid_h-lid_t-0.1]) cube([lcd_aperture_w,lcd_aperture_h,lid_t+0.3]);
    // Button guide hole + recessed cap pocket.
    translate([button_x,button_y,lid_h-lid_t-0.1]) cylinder(h=lid_t+0.3,d=button_hole_d);
    translate([button_x,button_y,lid_h-button_recess_depth]) cylinder(h=button_recess_depth+0.2,d=button_recess_d);
    // Potentiometer access through left lid wall at same shaft centreline.
    translate([-1,pot_y,pot_axis_z-base_h]) rotate([0,90,0]) cylinder(h=wall+lip_t+lid_clearance+2,d=pot_wall_hole_d);
  }
}

module lcd_display_tunnel() {
  tunnel_bottom_local = lcd_tunnel_bottom_z-base_h;
  tunnel_top_local = lid_h-lid_t+0.1;
  tunnel_h = tunnel_top_local-tunnel_bottom_local;
  if (tunnel_h>0) difference() {
    translate([lcd_window_x0-lcd_bezel_wall,lcd_window_y0-lcd_bezel_wall,tunnel_bottom_local])
      cube([lcd_aperture_w+2*lcd_bezel_wall,lcd_aperture_h+2*lcd_bezel_wall,tunnel_h]);
    translate([lcd_window_x0,lcd_window_y0,tunnel_bottom_local-0.1]) cube([lcd_aperture_w,lcd_aperture_h,tunnel_h+0.2]);
  }
}

module plunger_guide_tube() {
  guide_bottom_local = lid_h-lid_t-plunger_guide_len;
  difference() {
    translate([button_x,button_y,guide_bottom_local]) cylinder(h=plunger_guide_len+0.1,d=plunger_guide_od);
    translate([button_x,button_y,guide_bottom_local-0.1]) cylinder(h=plunger_guide_len+0.2,d=button_hole_d);
    translate([button_x,button_y,guide_bottom_local-0.01]) cylinder(h=0.55,d1=button_hole_d+0.7,d2=button_hole_d);
  }
}

module lid_part() { union() { lid_shell(); lcd_display_tunnel(); plunger_guide_tube(); } }

module button_plunger_part() {
  difference() {
    union() {
      cylinder(h=0.8,d1=3.6,d2=plunger_stem_d);
      translate([0,0,0.8]) cylinder(h=plunger_stem_len-0.8,d=plunger_stem_d);
      translate([0,0,plunger_stem_len]) cylinder(h=plunger_head_h,d=plunger_head_d);
    }
    translate([0,0,plunger_clip_groove_z-plunger_clip_t/2])
      difference() {
        cylinder(h=plunger_clip_t,d=plunger_stem_d+0.15);
        translate([0,0,-0.1]) cylinder(h=plunger_clip_t+0.2,d=plunger_clip_id-0.1);
      }
  }
}

module button_retaining_clip_part() {
  difference() {
    cylinder(h=plunger_clip_t,d=plunger_clip_od);
    translate([0,0,-0.1]) cylinder(h=plunger_clip_t+0.2,d=plunger_clip_id);
    translate([0,-plunger_clip_gap/2,-0.1]) cube([plunger_clip_od/2+0.2,plunger_clip_gap,plunger_clip_t+0.2]);
  }
}

module pot_shaft_extension_part() {
  total_len = pot_extension_external_len+wall+0.4;
  difference() {
    union() {
      cylinder(h=total_len,d=pot_extension_od);
      translate([0,0,pot_extension_external_len-pot_extension_collar_len]) cylinder(h=pot_extension_collar_len,d=pot_extension_collar_d);
    }
    translate([0,0,-0.1]) cylinder(h=total_len+0.2,d=pot_extension_bore_d);
  }
}

module reference_components() {
  if (show_components) {
    color([0.10,0.50,0.25,0.75]) translate([pcb_x0,pcb_y0,pcb_z]) cube([pcb_w,pcb_l,pcb_t]);
    color([0.10,0.42,0.82,0.9]) translate([lcd_x0,lcd_y0,lcd_body_bottom_z]) cube([lcd_module_w,lcd_module_d,lcd_body_thickness]);
    color([0.85,0.42,0.10,0.45]) translate([uno_x0,uno_y0,pcb_top_z]) cube([uno_w,uno_l,uno_z_envelope]);
    color([0.72,0.30,0.72,0.8]) translate([pot_body_start_x,pot_y,pot_axis_z]) rotate([0,90,0]) cylinder(h=pot_body_length,d=pot_body_d);
    color([0.95,0.65,0.10,0.95]) translate([button_x-button_d/2,button_y-button_d/2,button_top_z-2.0]) cube([button_d,button_d,2.0]);
  }
}

module assembly_view() {
  color([0.72,0.75,0.78,1]) base_tray();
  color([0.86,0.88,0.90,0.92]) translate([0,0,base_h]) lid_part();
  reference_components();
  // Exactly one actuator and one retaining clip.
  color([0.95,0.72,0.15,1]) translate([button_x,button_y,button_top_z+button_tip_gap]) button_plunger_part();
  color([0.25,0.25,0.28,1]) translate([button_x,button_y,button_top_z+button_tip_gap+plunger_clip_groove_z-plunger_clip_t/2]) button_retaining_clip_part();
  // Shaft extension points outward through the left wall.
  color([0.25,0.25,0.28,1]) translate([-pot_extension_external_len,pot_y,pot_axis_z]) rotate([0,90,0]) pot_shaft_extension_part();
}

if (part=="base") base_tray();
else if (part=="lid") translate([0,0,lid_overlap]) lid_part();
else if (part=="button_plunger") button_plunger_part();
else if (part=="button_retaining_clip") button_retaining_clip_part();
else if (part=="pot_shaft_extension") pot_shaft_extension_part();
else assembly_view();
