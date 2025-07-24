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

module recortado(){
    union(){
        difference(){
            un_soporte();
            translate([4, -30, 0]) cube([5,30,7]);
            translate([0, -5, 0]) cube([53,6,20]);
            translate([40, 5, 16]) rotate([90,0,0]) cylinder(10, d=33);
        }
        
        translate([63.5, -5, 0]) cube([8, 5, 30]);
        //translate([9, -22, 0]) cube([3,5,5]);
        //translate([15, -13, 0]) cube([10,3,5]);
    }
}

//recortado();
module medio_ojal(){
    intersection(){
    translate([-11, 12.25, 0]) un_soporte();
    translate([-0.05, -6.75, 0]) cube([6.1,13.5,4]);
    }
}

module ojal(){
    union(){
        medio_ojal();
        mirror([1,0,0]) medio_ojal();
    }}

// ojal();
module soporte_sin_ojales(){
difference(){
    union(){
        translate([-10, 12.25, 0]) recortado();
        translate([-1, -10, 0]) cube([10, 15, 3]);
        translate([56, -10, 0]) cube([10, 15, 3]);
    }
    translate([-1, -8, 3]) cube([10, 15, 3]);
    translate([55, -8, 3]) cube([10, 15, 3]);
}
}
// uncomment to draw as reference only
// motor();

module punta_con_ojal(){
    intersection(){
recortado();
translate([0, -30, -1]) cube([22, 40, 7]);}
}

module soporte_sin_punta(){
    difference(){
recortado();
translate([0, -30, -1]) cube([22, 40, 7]);}
}

module soporte_nuevo(){
    union(){
soporte_sin_punta();
translate([5,0,0]) punta_con_ojal();}
}



module lego_13(){import("./13.stl");}

module lego_13_cut(){
intersection(){
translate([5, 1, 0]) lego_13();
    cube([25, 10, 10]);
}
}



// as reference a lego beam
// https://www.printables.com/model/844733-lego-technic-beams-set-1-15/files
// translate([0,-16,0]) import("./13.stl");

soporte_nuevo();
