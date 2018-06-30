
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity MSP430_i2c_fault_detect is
    port (
        CLK: in std_logic;
        I2C_WRITE_FLAG: in std_logic;--from the i2c write bus
        RESET_FROM_PROCESSOR: in std_logic;
        RESET_TO_MSP430: out std_logic;
        RESET_ISSUED: out std_logic
   
    );
end MSP430_i2c_fault_detect;

architecture MSP430_i2c_fault_detect_Behavioral of MSP430_i2c_fault_detect is
    CONSTANT  upper_transmission_time_sending     :  INTEGER := 1250000; 
    CONSTANT  upper_transmission_time_NOT_sending :  INTEGER := 12500000; 
    signal    write_flag_is_up_counter            :  INTEGER range 0 to upper_transmission_time_sending;
    signal    write_flag_is_down_counter          :  INTEGER range 0 to upper_transmission_time_NOT_sending;
    signal    reset_out                           :  std_logic;
begin

    counterProcess: process(I2C_WRITE_FLAG,CLK ,RESET_FROM_PROCESSOR)
    begin
    -- reset if IIC transmittion is more then 10 ms or no transmittion for more then 100 ms
        if rising_edge(CLK) then
            if ((I2C_WRITE_FLAG = '0') and (RESET_FROM_PROCESSOR = '0')) then
                write_flag_is_up_counter <= 0;
                write_flag_is_down_counter <= 0;
                reset_out <= '1'; -- normal state
            elsif (RESET_FROM_PROCESSOR = '1') then
               reset_out <= '0'; -- normal state   
            -- Case There are No IIC transmissions
            elsif ((I2C_WRITE_FLAG = '0')) then 
                write_flag_is_up_counter <= 0;
                if write_flag_is_down_counter < upper_transmission_time_NOT_sending then
                   write_flag_is_down_counter <= write_flag_is_up_counter + 1;
                else
                    write_flag_is_down_counter <= 0;
                    reset_out <= '0';
                end if;
            -- Case There are IIC transmissions
            else
                write_flag_is_down_counter <= 0;
                if write_flag_is_up_counter < upper_transmission_time_sending then
                   write_flag_is_up_counter <= write_flag_is_up_counter + 1;
                else
                    write_flag_is_up_counter <= 0;
                    reset_out <= '0';
                end if;
            end if;
        end if;
    end process;

    RESET_TO_MSP430 <= reset_out;
    RESET_ISSUED <= reset_out;

end MSP430_i2c_fault_detect_Behavioral;
