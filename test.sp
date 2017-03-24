(
_seq "./tracker3.so" fl
_clk var
0 120 4 clock _clk set

_clk get _notes _gates "test.rnt" _seq fe

0 _notes tget "val" print mtof 
0 _gates tget 0.5 0 thresh 
0.001 tport 
0.3 1 1 1 fm 

0 _gates tget 0.01 port *

_clk get 0.001 0.001 0.001 tenvx 1000 0.3 sine * +
dup
_seq fc
)
