# Detailed breakdown of bitwise representation

each face of the cube is made up of 9 stickers, but the center is stationary so only 8 need to be stored. And there are 6 colors, so each color fits in a byte. Given these color definitions:

enum class COLOR : uchar {WHITE, GREEN, RED, BLUE, ORANGE, YELLOW};
A face might look like this, stored in a single 64-bit integer:

00000000 00000001 00000010 00000011 00000100 00000101 00000000 00000001
Which is decoded as:

WGR  -->
G B    |
WYO  ^--
or 
00000000 00000001 00000010 00000011 00000100 00000101 00000000 00000001
   WHITE   GREEN    RED     BLUE     ORANGE   YELLOW    WHITE  GREEN
An advantage of using this structure is that the rolq and rorq bit-wise operators can be used to move a face. 
Rolling by 16 bits effects a 90-degree rotation; 
rolling by 32 bits gives a 180-degree turn. The adjacent pieces need to be up-kept manually
--i.e. after rotating the top face, the top layer of the front, left, back, and right faces need to be moved, too. 
Turning faces in this manner is really fast. For example, rolling

00000000 00000001 00000010 00000011 00000100 00000101 00000000 00000001
by 16 bits yields to the right 

00000100 00000101 00000000 00000001 00000000 00000001 00000010 00000011 
Decoded, that looks like this:

WGW
Y G
OBR

A 64 bit numbers for each face will be defined 

shifting for the moves
IF THE TURN IS 1#
- 24 bits need to be stored and copied to the next one. 
- ORDER MATTERS BE VERY CAREFEE
- use the correct mask(what values to save and rotate around)
- set the anti mask for each switch(what values to discard)
- shift around the face using th esame algo

*USING MASK LEFT NEEDS TO BE WITH CAUTION*
WRAPS AROUND!!


## rotation logic
- uses masks(things to save) and anti-masks(the thigns  to get rid or to move)
- generic turn and turn prime function for in face rotation.
