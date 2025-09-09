`timescale 1ns / 1ps
`define UD #1

module led_test(
    input          clk    ,
    input          rstn   ,
    
    output [7:0]   led  
);

    reg [24:0] led_light_cnt    = 25'd0         ;
    reg [ 7:0] led_status       = 8'b0000_0001  ;

    //0.5s
    always @(posedge clk)
    begin
        if(!rstn)
            led_light_cnt <= `UD 25'd0;
        else if(led_light_cnt == 25'd24_999_999)
            led_light_cnt <= `UD 25'd0;
        else
            led_light_cnt <= `UD led_light_cnt + 25'd1; 
    end

    always @(posedge clk)
    begin
        if(!rstn)
            led_status <= `UD 8'b0000_0001;
        else if(led_light_cnt == 25'd24_999_999)
            led_status <= `UD {led_status[6:0],led_status[7]};
    end

    assign led = led_status;
    
endmodule
