pixelWidth  = 23;
pixelHeight = 2.8;

battWidth  = 25.5;
battLength = 19.5;
battHeight = 4;
battWidthOffset = 1;

battPcbGap = 4;

pcbWidth  = 25.4;
pcbHeight = 1.75;

usbWidth  = 10;
usbLength = 6;
usbHeight = 3;

switchWidth  = 6;
switchLength = 10;
switchHeight = 3;

/////// Assembly

PendantComponents();








/////// Parts

module PendantComponents() {
  color([.75, .25, .25]) {
    // PCB
    cylinder(r=pcbWidth/2, h=pcbHeight, center=false, $fn=360);

    // USB Port
    translate([0, -(pcbWidth/2-usbLength/2), -usbHeight])
      hcCube(usbWidth, usbLength, usbHeight);

    // Power Switch
    translate([pcbWidth/2-switchWidth/2, 0, -usbHeight])
      hcCube(switchWidth, switchLength, switchHeight);

    // Battery gap
    translate([0, 0, pcbHeight])
      hcCube(10, 10, battPcbGap);

    // Battery
    translate([-battWidthOffset, 0, pcbHeight + battPcbGap])
      hcCube(battWidth, battLength, battHeight);

    // Pixels
    translate([0, 0, pcbHeight + battPcbGap + battHeight])
      cylinder(r=pixelWidth/2, h=pixelHeight, center=false, $fn=360);
  }
}




/////// Lib

module hcCube(w, l, h) {
  translate([-w/2, -l/2, 0])
  cube(size=[w, l, h], center=false);
}
