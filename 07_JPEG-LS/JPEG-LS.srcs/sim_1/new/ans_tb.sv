module ans_tb();

reg clk;
reg ce_encoder;
reg ce_decoder;
reg last_in_encoder;
reg [8:0] symbol_in;

wire [8:0] symbol_out;
wire last_out;

ans a(clk, ce_encoder, ce_decoder, last_in_encoder, symbol_in, symbol_out, last_out);

always 
begin
    #5 
    clk <= ~clk;
end

initial
begin 
    clk <= 0;
    last_in_encoder <= 0;
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
    symbol_in <= 9'b000000000;
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
