union(){
    translate([-10, 21.2, -10]) import("/home/seba/Downloads/Cilindro (forma de reloj de arena).stl");

e = 1.8; // el ancho de una aletita de eje en cruz
E = 4.64; // ancho total del eje

ecc = e / 2 + 0.1; // cuanto desplazar el cubito del eje central

translate([ecc, ecc, 0]) cube([5,5,5]);
translate([ecc, ecc, 65]) cube([5,5,5]);
}