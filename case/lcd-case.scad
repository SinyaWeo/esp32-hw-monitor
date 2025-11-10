//Front plate width
width=90;
//Front plate height
height=30;
//Front plate thickness
thickness=1;
//Front plate corner radius
corner_radius=2;
corner_faces=64;

//Cutout width
cutout_width=58;
//Cutout height
cutout_height=18;
//Cutout corner radius
cutout_corner_radius=0;
cutout_corner_faces=32;
//Horizontal cutout offset from the center of the front plate
cutout_offset_x=-4; //from the center of the front plate
//Vertical cutout offset from the center of the front plate
cutout_offset_y=0; //from the center of the front plate

//Post base radius
post_base_radius=3;
//Post top radius
post_top_radius=2.5;
//Post hole radius
post_hole_radius=1.5;
//Post offsett from case edge
post_offset=4;
//Post support thickness
post_support_thickness=1;

//Rim (case) height 
rim_height=30;
//Rim thickness
rim_thickness=1;

//Backplate thickness
backplate_thickness=1;

parts_immersion=0.2;

//

module body (sizeX, sizeY, sizeZ, cornerRadius, cornerFaces)
{
	union()
	{
		difference()
		{
			cube([sizeX,sizeY,sizeZ]);		
			cube([cornerRadius,cornerRadius,sizeZ+thickness]);
			translate([sizeX-cornerRadius,0,0]) cube([cornerRadius,cornerRadius,sizeZ+thickness]);
			translate([0,sizeY-cornerRadius,0]) cube([cornerRadius,cornerRadius,sizeZ+thickness]);
			translate([sizeX-cornerRadius,sizeY-cornerRadius,0]) cube([cornerRadius,cornerRadius,sizeZ+thickness]);
		}
		translate([cornerRadius,cornerRadius,0]) cylinder(r=cornerRadius,h=sizeZ, $fn=cornerFaces);
		translate([sizeX-cornerRadius,cornerRadius,0]) cylinder(r=cornerRadius,h=sizeZ, $fn=cornerFaces);
		translate([cornerRadius,sizeY-cornerRadius,0]) cylinder(r=cornerRadius,h=sizeZ, $fn=cornerFaces);
		translate([sizeX-cornerRadius,sizeY-cornerRadius,0]) cylinder(r=cornerRadius,h=sizeZ, $fn=cornerFaces);
	}
}

module backplate (sizeX, sizeY, cornerRadius, cornerFaces)
{

    difference()
    {
        union()
        {
            difference()
            {
                cube([sizeX,sizeY,backplate_thickness]);		
                cube([cornerRadius,cornerRadius,backplate_thickness]);
                translate([sizeX-cornerRadius,0,0]) cube([cornerRadius,cornerRadius,backplate_thickness]);
                translate([0,sizeY-cornerRadius,0]) cube([cornerRadius,cornerRadius,backplate_thickness]);
                translate([sizeX-cornerRadius,sizeY-cornerRadius,0]) cube([cornerRadius,cornerRadius,backplate_thickness]);

            }
            translate([cornerRadius,cornerRadius,0]) cylinder(r=cornerRadius,h=backplate_thickness, $fn=cornerFaces);
            translate([sizeX-cornerRadius,cornerRadius,0]) cylinder(r=cornerRadius,h=backplate_thickness, $fn=cornerFaces);
            translate([cornerRadius,sizeY-cornerRadius,0]) cylinder(r=cornerRadius,h=backplate_thickness, $fn=cornerFaces);
            translate([sizeX-cornerRadius,sizeY-cornerRadius,0]) cylinder(r=cornerRadius,h=backplate_thickness, $fn=cornerFaces);
        }

        backplate_holes();
	}
}

module left_post_supports()
{
        //left top support
        translate([0+rim_thickness-parts_immersion,0+post_offset-post_support_thickness/2,thickness]) cube([post_offset-rim_thickness-post_top_radius+parts_immersion*2,post_support_thickness,rim_height-thickness-backplate_thickness]);
        //left bottom support
        translate([0+rim_thickness-parts_immersion,height-post_offset-post_support_thickness/2,thickness]) cube([post_offset-rim_thickness-post_top_radius+parts_immersion*2,post_support_thickness,rim_height-thickness-backplate_thickness]);
    
}

module bottom_post_supports()
{
        //left top support
        translate([0+post_offset-post_support_thickness/2,0+rim_thickness-parts_immersion,thickness]) cube([post_support_thickness,post_offset-rim_thickness-post_top_radius+parts_immersion*2,rim_height-thickness-backplate_thickness]);
        //left bottom support
        translate([0+width-post_offset-post_support_thickness/2,rim_thickness-parts_immersion,thickness]) cube([post_support_thickness,post_offset-rim_thickness-post_top_radius+parts_immersion*2,rim_height-thickness-backplate_thickness]);
}



module posts()
{

	union()
	{
		difference()
		{
            //left bottom post
			translate([0+post_offset,0+post_offset,thickness]) cylinder(r1=post_base_radius,r2=post_top_radius,h=rim_height-thickness-backplate_thickness, $fn=32);	
			translate([0+post_offset,0+post_offset,thickness]) cylinder(r=post_hole_radius,h=rim_height-thickness+0.5, $fn=16);
		}
        
		difference()
		{
            //right bottom post
			translate([width-post_offset,0+post_offset,thickness]) cylinder(r1=post_base_radius,r2=post_top_radius,h=rim_height-thickness-backplate_thickness, $fn=32);	
			translate([width-post_offset,0+post_offset,thickness]) cylinder(r=post_hole_radius,h=rim_height-thickness+0.5, $fn=16);
		}

		difference()
		{
            //left top post
			translate([0+post_offset,height-post_offset,thickness]) cylinder(r1=post_base_radius,r2=post_top_radius,h=rim_height-thickness-backplate_thickness, $fn=32);	
			translate([0+post_offset,height-post_offset,thickness]) cylinder(r=post_hole_radius,h=rim_height-thickness+0.5, $fn=16);
		}	

		difference()
		{
            //right top post
			translate([width-post_offset,height-post_offset,thickness]) cylinder(r1=post_base_radius,r2=post_top_radius,h=rim_height-thickness-backplate_thickness, $fn=32);	
			translate([width-post_offset,height-post_offset,thickness]) cylinder(r=post_hole_radius,h=rim_height-thickness+0.5, $fn=16);
		}
        
        left_post_supports();
        translate([width,0,0]) mirror([1,0,0]) left_post_supports();
        bottom_post_supports();
        translate([0,height,0]) mirror([0,1,0]) bottom_post_supports();
	}
}

module backplate_holes()
{

	union()
	{
            //left bottom hole
			translate([0+post_offset-rim_thickness,0+post_offset-rim_thickness,-0.5]) cylinder(r=post_hole_radius,h=backplate_thickness+1, $fn=16);
            //right bottom hole
			translate([width-post_offset-rim_thickness,0+post_offset-rim_thickness,-0.5]) cylinder(r=post_hole_radius,h=backplate_thickness+1, $fn=16);
            //left top hole
			translate([0+post_offset-rim_thickness,height-post_offset-rim_thickness,-0.5]) cylinder(r=post_hole_radius,h=backplate_thickness+1, $fn=16);
            //right top hole
			translate([width-post_offset-rim_thickness,height-post_offset-rim_thickness,-0.5]) cylinder(r=post_hole_radius,h=backplate_thickness+1, $fn=16);
	}
}

difference()
{
    body (width, height,rim_height, corner_radius, corner_faces);
    translate([rim_thickness,rim_thickness,0])
    body (width-(2*rim_thickness), height-(2*rim_thickness),rim_height, corner_radius-rim_thickness, corner_faces);
}

union()
{
    difference()
    {
        //base body
        body (width, height, thickness, corner_radius, corner_faces);

        //display cutout
        translate([0+(width-cutout_width)/2+cutout_offset_x,0+(height-cutout_height)/2+cutout_offset_y,0])
        body(cutout_width,cutout_height,10,cutout_corner_radius,cutout_corner_faces);

    }
    //posts
    translate([0,0,0])
    posts();
    translate([0,height+10,0])

    //backplate
    backplate(width-2*rim_thickness, height-2*rim_thickness, corner_radius-rim_thickness, corner_faces);
}


