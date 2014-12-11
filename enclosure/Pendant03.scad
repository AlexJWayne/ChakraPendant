pixelWidth  = 23;
pixelHeight = 2.8;

battWidth  = 25.5;
battLength = 19.5;
battHeight = 4;
battClearance = 0.6;

battPcbGap = 4;

pcbWidth  = 25.4;
pcbHeight = 1.75;

usbWidth  = 10;
usbLength = 6;
usbHeight = 3;

switchWidth  = 6;
switchLength = 10;
switchHeight = 3;

bodyWidth  = 34;
bodyHeight = 19;
bodyWall   = 1.6;
bodyBezel  = 3.5;

/////// Assembly

PendantComponents();

Body();






/////// Parts

module Body() {
  difference() {
    // main body
    cylinder(r=bodyWidth/2, h=bodyHeight, center=false, $fn=360);

    // hollow out
    translate([0, 0, -bodyWall])
      cylinder(r=bodyWidth/2 - bodyWall, h=bodyHeight, center=false, $fn=360);

    // Cut out diffuser
    translate([0, 0, bodyHeight - bodyWall*2])
      cylinder(r=bodyWidth/2 - bodyBezel, h=bodyWall*6, center=true, $fn=360);

    // battery corners
    translate([0, 0, -bodyWall])
      hcCube(
        battWidth  + battClearance*2,
        battLength + battClearance*2,
        bodyHeight
      );
  }
}

module PendantComponents() {
  color([.75, .25, .25]) {
    // USB Port
    translate([0, -(pcbWidth/2-usbLength/2), ])
      hcCube(usbWidth, usbLength, usbHeight);

    // Power Switch
    translate([pcbWidth/2-switchWidth/2, 0, 0])
      hcCube(switchWidth, switchLength, switchHeight);

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
