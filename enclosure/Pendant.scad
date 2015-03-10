pixelWidth  = 25.4;
pixelHeight = 2.6;

battWidth  = 25.5;
battLength = 19.5;
battHeight = 4;
battClearance = 0.3;

battPcbGap = 0;

pcbWidth  = 25.4;
pcbHeight = 1.75;

usbWidth  = 10;
usbLength = 6;
usbHeight = 3;

switchWidth  = 6;
switchLength = 10;
switchHeight = 3;

bodyWidth  = 35;
bodyHeight = 16;
bodyWall   = 1.6;
bodyBezel  = 6;

/////// Assembly

/*PendantComponents();*/

rotate([180, 0, 0])
  union() {
    /*difference() { Body(); cube([100, 100, 100]); }*/
    Body();

    /*// hook tabs
    translate([0, pcbWidth/2 + 0.25, usbHeight-0.5])
      scale([2, 1, 1])
        tab();

    translate([-battWidth/2 - 0.5, 0, usbHeight-0.5])
      scale([1, 2, 1])
        tab();*/
  }






/////// Parts

module Body() {
  intersection() {
    difference() {
      // main body
      cylinder(r=bodyWidth/2, h=bodyHeight, center=false, $fn=360);

      // pixel pcb hollow
      translate([0, 0, -bodyWall])
      cylinder(r=pixelWidth/2 + .5, h=bodyHeight, center=false, $fn=360);

      // Cut out diffuser
      translate([0, 0, bodyHeight - bodyWall*2])
      cylinder(r=bodyWidth/2 - bodyBezel, h=bodyWall*6, center=true, $fn=360);

      // battery corners
      translate([0, 0, -(bodyWall + pixelHeight + 2)])
      hcCube(
        battWidth  + battClearance*2,
        battLength + battClearance*2,
        bodyHeight
      );

      // main PCB
      translate([0, 0, -1])
      cylinder(r=pcbWidth/2, h=(usbHeight + pcbHeight + 1), center=false, $fn=360);

      // USB cutout
      translate([0, -(pcbWidth - usbLength)/2 - 5, 0])

      /*// Power Switch cutout
      translate([pcbWidth/2-switchWidth/2 + 5, 0, 0])
      hcCube(switchWidth + 4, switchLength + 3, switchHeight + 3);*/

      // chain channel
      translate([0, 14, 3])
      hcCube(50, 2, 4);
    }

    // Chamfer top edge
    cylinder(r1=bodyWidth * 0.74, r2=0, h=bodyWidth * 1, center=false, $fn=360);
  }
}

module tab() {
  cylinder(r1=.6, r2=0, h=1, $fn=180);
}

module PendantComponents() {

  translate([0, 0, 1])
  color([.75, .25, .25]) {
    // USB Port
    translate([0, -(pcbWidth/2-usbLength/2), ])
      hcCube(usbWidth, usbLength, usbHeight);

    /*// Power Switch
    translate([pcbWidth/2-switchWidth/2, 0, 0])
      hcCube(switchWidth, switchLength, switchHeight);*/

    // PCB
    translate([0, 0, usbHeight])
      cylinder(r=pcbWidth/2, h=pcbHeight, center=false, $fn=360);


    // Battery gap
    translate([0, 0, usbHeight + pcbHeight])
      hcCube(10, 10, battPcbGap);

    // Battery
    translate([0, 0, usbHeight + pcbHeight + battPcbGap])
      hcCube(battWidth, battLength, battHeight);

    // Pixels
    translate([0, 0, usbHeight + pcbHeight + battPcbGap + battHeight])
      cylinder(r=pixelWidth/2, h=pixelHeight, center=false, $fn=360);
  }
}




/////// Lib

module hcCube(w, l, h) {
  translate([-w/2, -l/2, 0])
  cube(size=[w, l, h], center=false);
}
