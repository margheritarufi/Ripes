//Single stage cpu top level

`include "ripes_params.vh"
`include "designer_params.vh"


module cpu
#(  parameter ADDR_WIDTH = `ADDR_WIDTH,
	parameter DATA_WIDTH = `DATA_WIDTH,
	parameter TGDI_WIDTH = `TGDI_WIDTH,
	parameter TGDO_WIDTH = `TGDO_WIDTH,
	parameter TGA_WIDTH = `TGA_WIDTH,
	parameter TGC_WIDTH = `TGC_WIDTH,
	parameter SEL_WIDTH = `SEL_WIDTH
)
(
	input clk_i,
	input rst_i,
	
	output [ADDR_WIDTH-1:0]adr_o,
	
	input [DATA_WIDTH-1:0]dat_i,
	output [DATA_WIDTH-1:0]dat_o,
	
	input ack_i, err_i, rty_i,
	output cyc_o, stb_o, lock_o, we_o,
	output [SEL_WIDTH-1:0] sel_o,
	
	input [TGDI_WIDTH-1:0]tgd_i,
	output [TGDO_WIDTH-1:0] tgd_o,
	output [TGA_WIDTH-1:0] tga_o,
	output [TGC_WIDTH-1:0] tgc_o
);

endmodule
