
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity Split_Axi is
    port (
         AXI_PS_TO_PL_1_0: in std_logic_vector(31 downto 0);
         AXI_PS_TO_PL_1_1: in std_logic_vector(31 downto 0);
         RESET_ISSUED: in std_logic;
         PWM0 :out std_logic_vector(7 downto 0);
         PWM1 :out std_logic_vector(7 downto 0);
         PWM2 :out std_logic_vector(7 downto 0);
         PWM3 :out std_logic_vector(7 downto 0);
         ThrottlePWM  :out std_logic_vector(7 downto 0);
         ThrottleError  :out std_logic_vector(7 downto 0);
         AXI_PL_TO_PS_0_0: out std_logic_vector(31 downto 0);
         RESET_TO_MSP430  : out std_logic 
         
    );
end Split_Axi;

architecture Split_Axi_Behavioral of Split_Axi is  
begin

    RESET_TO_MSP430 <= '0';
    PWM0(7 downto 0) <= AXI_PS_TO_PL_1_0(7 downto 0);
    PWM1(7 downto 0) <= AXI_PS_TO_PL_1_0(15 downto 8);
    PWM2(7 downto 0) <= AXI_PS_TO_PL_1_0(23 downto 16);
    PWM3(7 downto 0) <= AXI_PS_TO_PL_1_0(31 downto 24);
    ThrottlePWM(7 downto 0)  <= AXI_PS_TO_PL_1_1(7 downto 0);
    ThrottleError(7 downto 0)  <= AXI_PS_TO_PL_1_1(15 downto 8);

end Split_Axi_Behavioral;
