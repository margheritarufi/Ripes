//Memory module top level

`include "ripes_params.vh"
`include "designer_params.vh"


module memory
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
	
	input [ADDR_WIDTH-1:0]adr_i,
	
	input [DATA_WIDTH-1:0]dat_i,
	output [DATA_WIDTH-1:0]dat_o,
	
	output ack_o, err_o, rty_o,
	input cyc_i, stb_i, lock_i, we_i,
	input [SEL_WIDTH-1:0] sel_i,
	
	output [TGDO_WIDTH-1:0]tgd_o,
	input [TGDI_WIDTH-1:0] tgd_i,
	input [TGA_WIDTH-1:0] tga_i,
	input [TGC_WIDTH-1:0] tgc_i
);

endmodule
