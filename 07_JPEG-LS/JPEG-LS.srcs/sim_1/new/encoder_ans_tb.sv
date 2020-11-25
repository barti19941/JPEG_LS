module encoder_ans_tb();

reg clk;
reg ce_encoder;
reg ce_decoder;
reg [8:0] symbol_in;
reg last_in_encoder;

wire [4:0] counter [2:0];
wire [15:0] data [0:31];
wire [15:0] symbol;
wire [15:0] bits;
wire [4:0] nbbits;
wire valid [1:0];
wire [8:0] symbol_out;
wire last_out; 

encoder_ans e_a(clk, ce_encoder, symbol_in, last_in_encoder, valid[0], bits, nbbits);
byte_packer b_p(clk, valid[0], bits, nbbits, valid[1], symbol, counter[0]);
two_bytes_buffer t_b_b(clk, valid[1], symbol, counter[0], counter[1], counter[2], data);
decoder_ans d_a(clk, ce_decoder, data, counter[1], counter[2], symbol_out, last_out);
 
always 
begin
    #5 
    clk <= ~clk;
end

initial
begin 
    clk <= 0;
    last_in_encoder <= 0;
    ce_encoder <= 0;
    #10
    ce_encoder <= 1;
    symbol_in <= 0;
    #10
    symbol_in <= 5;
    #10
    symbol_in <= 2;
    #10
    symbol_in <= 3;
    #10
    symbol_in <= 4;
    #10
    symbol_in <= 5;
    #10
    symbol_in <= 6;
    #10
    symbol_in <= 7;
    #10
    symbol_in <= 8;
    #10
    symbol_in <= 9;
    #10
    symbol_in <= 10;
    #10
    symbol_in <= 15;
    #10
    symbol_in <= 20;
    #10
    symbol_in <= 25;
    #10
    symbol_in <= 30;
    #10
    symbol_in <= 40;
    #10
    symbol_in <= 50;
    #10
    symbol_in <= 60;
    #10
    symbol_in <= 70;
    #10
    symbol_in <= 80;
    #10
    symbol_in <= 90;
    #10
    symbol_in <= 100;
    #10
    symbol_in <= 150;
    #10
    symbol_in <= 200;
    #10
    symbol_in <= 250;
    #10    
    last_in_encoder <= 1;
    symbol_in <= 9'bZZZZZZZZZ;
    #10
    last_in_encoder <= 0;
    #10
    ce_encoder <= 0;
end

initial
begin
    ce_decoder <= 0;
    #300
    ce_decoder <= 1;
end

endmodule
