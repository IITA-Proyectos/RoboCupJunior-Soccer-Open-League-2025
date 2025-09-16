
module borne(){
    rotate([0, -90, 0]) translate([-5,0,0])
union(){
difference(){
    cube([10, 6.4, 0.8], center=true);
    cylinder(1.0, d=1.4, center=true, $fn=10);
}
translate([-8-3, 0 ,0]) cube([16, 8.0, 0.8], center=true);}}


module agujerito(){
    rotate([90,0,0]) 
        cylinder(4, d=2.5, $fn=10);    
    }

module agujeritos(){
    daux = (30.5 + 25.3) / 2;
    translate([       22, 3, 6.2 + 2.5/2]) agujerito();
    translate([22 + daux, 3, 6.2 + 2.5/2]) agujerito();
    translate([       22, 3, 35 - 6.2 - 2.5/2]) agujerito();
    translate([22 + daux, 3, 35 - 6.2 - 2.5/2]) agujerito();
}

module placalateral(){
    difference(){
        cube([71.0, 2, 35]);
        agujeritos();
    }
}

module solenoide(){
    union(){
        placalateral();
        translate([0,40,0]) 
            placalateral();

        cube([2, 42, 35]); // placa de atras
        translate([67,0,0]) 
            cube([2, 42, 35]); // placa de adelante

        translate([2, 42/2, 35/2]) 
            rotate([0, 90, 0]) 
                cylinder(h=66, d=34);

        // los dos bornes
        translate([25+0.4, 21, 17+35]) borne();
        translate([25+0.8+17, 21, 17+35]) borne();

        // el eje saliendo adelante
        translate([67+15, 42/2, 35/2])
            rotate([0, -90, 0]) 
                cylinder(h=20, d=15);

    }
}

solenoide();