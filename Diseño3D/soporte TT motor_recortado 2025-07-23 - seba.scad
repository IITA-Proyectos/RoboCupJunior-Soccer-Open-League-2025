// https://www.thingiverse.com/thing:5611259
module un_soporte(){
    rotate([90,0,0])
    difference(){
    import("./TT Motor mount - 5611259/files/6VDC_motor_mount_REV2.STL");
    translate([-1,-5, 27]) cube([100, 40, 30]);
    }    
}


// https://www.thingiverse.com/thing:1079893
module motor(){
    translate([93,4,27.5])
    rotate([90,180,0]) import("./yellow TT motor - 1079893/files/DC_Motor_20mm.STL");
    }

union(){
    difference(){
        un_soporte();
        translate([4, -30, 0]) cube([5,30,7]);
        translate([0, -5, 0]) cube([53,6,20]);
        translate([40, 5, 16]) rotate([90,0,0]) cylinder(10, d=33);
    }
    
    translate([63.5, -5, 0]) cube([8, 5, 30]);
    translate([9, -22, 0]) cube([3,5,5]);
    translate([15, -13, 0]) cube([10,3,5]);
}
// uncomment to draw as reference only
// motor();