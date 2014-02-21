{\rtf1\ansi\ansicpg1252\cocoartf1265
{\fonttbl\f0\fnil\fcharset0 Consolas;\f1\fnil\fcharset0 Consolas-Bold;\f2\fnil\fcharset0 Consolas-Italic;
}
{\colortbl;\red255\green255\blue255;\red147\green5\blue19;\red220\green199\blue200;\red38\green38\blue38;
\red14\green110\blue109;\red210\green0\blue53;\red135\green136\blue117;\red53\green65\blue117;\red133\green0\blue2;
\red17\green137\blue135;}
{\info
{\author Red Byer}
{\*\company Red's Toyland}
{\*\copyright 2011, Red's Toyland.  ALL RIGHTS RESERVED}}\margl1440\margr1440\vieww10800\viewh8400\viewkind0
\deftab720
\pard\pardeftab720

\f0\fs24 \cf2 \cb3 #\cf4 \cb1 include 
\f1\b <
\f0\b0 Wire
\f1\b .
\f0\b0 \cf5 h
\f1\b \cf4 >
\f0\b0 \
\cf2 \cb3 #\cf4 \cb1 include \cf6 "Adafruit_MCP23017.h"\cf4 \
\
\pard\pardeftab720

\f2\i \cf7 // Basic pin reading and pullup test for the MCP23017 I/O expander
\f0\i0 \cf4 \

\f2\i \cf7 // public domain!
\f0\i0 \cf4 \
\

\f2\i \cf7 // Connect pin #12 of the expander to Analog 5 (i2c clock)
\f0\i0 \cf4 \

\f2\i \cf7 // Connect pin #13 of the expander to Analog 4 (i2c data)
\f0\i0 \cf4 \

\f2\i \cf7 // Connect pins #15, 16 and 17 of the expander to ground (address selection)
\f0\i0 \cf4 \

\f2\i \cf7 // Connect pin #18 of the expander to 5V THROUGH a 10K resistor (RESET)
\f0\i0 \cf4 \

\f2\i \cf7 //     if you do not, the expander may fail unexpectedly in several hours or days of use!
\f0\i0 \cf4 \

\f2\i \cf7 // Connect pin #9 of the expander to 5V (power)
\f0\i0 \cf4 \

\f2\i \cf7 // Connect pin #10 of the expander to ground (common ground)
\f0\i0 \cf4 \
\

\f2\i \cf7 // Input #0 is on pin 21 so connect a button or switch from there to ground
\f0\i0 \cf4 \
\
Adafruit_MCP23017 mcp
\f1\b ;
\f0\b0 \
\'a0\'a0\
\pard\pardeftab720

\f1\b \cf8 void
\f0\b0 \cf4  
\f1\b \cf9 setup\cf4 ()
\f0\b0  
\f1\b \{
\f0\b0   \
\'a0\'a0mcp
\f1\b .
\f0\b0 \cf5 begin
\f1\b \cf4 ();
\f0\b0       
\f2\i \cf7 // use default address 0
\f0\i0 \cf4 \
\
\'a0\'a0mcp
\f1\b .
\f0\b0 \cf5 pinMode
\f1\b \cf4 (
\f0\b0 \cf10 0
\f1\b \cf4 ,
\f0\b0  INPUT
\f1\b );
\f0\b0 \
\'a0\'a0mcp
\f1\b .
\f0\b0 \cf5 pullUp
\f1\b \cf4 (
\f0\b0 \cf10 0
\f1\b \cf4 ,
\f0\b0  HIGH
\f1\b );
\f0\b0   
\f2\i \cf7 // turn on a 100K pullup internally
\f0\i0 \cf4 \
\
\'a0\'a0pinMode
\f1\b (
\f0\b0 \cf10 13
\f1\b \cf4 ,
\f0\b0  OUTPUT
\f1\b );
\f0\b0   
\f2\i \cf7 // use the p13 LED as debugging
\f0\i0 \cf4 \
\pard\pardeftab720

\f1\b \cf4 \}
\f0\b0 \
\
\
\
\pard\pardeftab720

\f1\b \cf8 void
\f0\b0 \cf4  
\f1\b \cf9 loop\cf4 ()
\f0\b0  
\f1\b \{
\f0\b0 \
\'a0\'a0
\f2\i \cf7 // The LED will 'echo' the button
\f0\i0 \cf4 \
\'a0\'a0digitalWrite
\f1\b (
\f0\b0 \cf10 13
\f1\b \cf4 ,
\f0\b0  mcp
\f1\b .
\f0\b0 \cf5 digitalRead
\f1\b \cf4 (
\f0\b0 \cf10 0
\f1\b \cf4 ));
\f0\b0 \
\pard\pardeftab720

\f1\b \cf4 \}}