library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity mux is
    Port ( A : in STD_LOGIC_VECTOR (31 downto 0);
           B : in STD_LOGIC_VECTOR (31 downto 0);
           sel : in STD_LOGIC;
           S : out STD_LOGIC_VECTOR (31 downto 0));
end mux;

architecture Behavioral of mux is

signal sa : STD_LOGIC;
signal sb : STD_LOGIC;
signal ap : STD_LOGIC_VECTOR (31 downto 0);
signal bp : STD_LOGIC_VECTOR (31 downto 0);

begin

sa <= not sel;
sb <= sel;

ap <= A and sa & sa & sa & sa & sa & sa & sa & sa &
    sa & sa & sa & sa & sa & sa & sa & sa &
    sa & sa & sa & sa & sa & sa & sa & sa &
    sa & sa & sa & sa & sa & sa & sa & sa;
    
bp <= B and sb & sb & sb & sb & sb & sb & sb & sb &
    sb & sb & sb & sb & sb & sb & sb & sb &
    sb & sb & sb & sb & sb & sb & sb & sb &
    sb & sb & sb & sb & sb & sb & sb & sb;
    
S <= ap or bp;

end Behavioral;
