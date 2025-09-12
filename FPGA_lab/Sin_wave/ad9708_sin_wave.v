module ad9708_sin_wave(
   input    wire  clk_50M        ,
   output   wire  da_clk         ,
   output   wire  [7:0]da_data   
   );

wire rst_n ;
wire clk_125M ;

reg   [9:0]rom_addr ;
wire  [7:0]rom_data_out ;

assign da_clk = clk_125M  ;
assign da_data = rom_data_out  ;

always @(negedge clk_125M or negedge rst_n) begin
   if (!rst_n)
      rom_addr <= 10'd0 ;
   else 
      rom_addr <= rom_addr + 10'd1     ;                //The output sine wave frequency is 122Khz
//    rom_addr <= rom_addr + 10'd2     ;                //The output sine wave frequency is 244Khz
//    rom_addr <= rom_addr + 10'd4     ;                //The output sine wave frequency is 488Khz
//    rom_addr <= rom_addr + 10'd32    ;                //The output sine wave frequency is 3.9Mhz
//    rom_addr <= rom_addr + 10'd128   ;                //The output sine wave frequency is 15.6Mhz
end

ad_clock_125m u_pll (
  .clkin1(clk_50M),          // input
  .pll_lock(rst_n),          // output
  .clkout0(clk_125M)         // output
);

rom_sin_wave u_rom (
  .addr(rom_addr),           // input [9:0]
  .clk(clk_125M),            // input
  .rst(1'b0),                // input
  .rd_data(rom_data_out)     // output [7:0]
);




endmodule