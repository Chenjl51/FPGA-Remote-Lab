`timescale 1ns / 1ps

`define UD #1
module key_led_top(
    input           clk,//50MHz
    input           key,
    
    output [7:0]    led
);

   wire [1:0] ctrl;
   
   key_ctl key_ctl(
       .clk        (  clk  ),//input            clk,
       .key        (  key  ),//input            key,
                 
       .ctrl       (  ctrl  )//output     [1:0] ctrl
   );
   
   led u_led(
       .clk   (  clk   ),//input         clk,
       .ctrl  (  ctrl  ),//input  [1:0]  ctrl,
                      
       .led   (  led   ) //output [7:0]  led
   );

endmodule
