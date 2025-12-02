library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
use IEEE.NUMERIC_STD.ALL;
use ieee.fixed_pkg.all;


-- Componente maior que engloba todos
entity acelerador_de_inferencias is
    port (
        entrada   : in  std_logic_vector(15 downto 0);  -- Entrada de 16 bits
        saida     : out std_logic                       -- Saída binária
    );
end entity;

architecture rtl of acelerador_de_inferencias is
    -- Definindo sinais para conectar os conversores de 4 bits para fixed
    signal valor_convertido : sfixed(7 downto -8) vector(0 to 3);  -- 4 valores de saída do conversor
    signal camada0_saida   : sfixed(7 downto -8) vector(0 to 3);  -- Saídas da camada 0 (neurônios)
    signal camada1_saida   : sfixed(7 downto -8) vector(0 to 2);  -- Saídas da camada 1 (neurônios)
    signal camada2_saida   : sfixed(7 downto -8);                  -- Saída da camada 2 (último neurônio)

begin
    -- Conversores de 4 bits para sfixed
    u_convert0 : entity work.convert4bits_to_fixed
        port map (
            bits4  => entrada(3 downto 0),   -- 1º grupo de 4 bits
            valor  => valor_convertido(0)
        );
    
    u_convert1 : entity work.convert4bits_to_fixed
        port map (
            bits4  => entrada(7 downto 4),   -- 2º grupo de 4 bits
            valor  => valor_convertido(1)
        );
    
    u_convert2 : entity work.convert4bits_to_fixed
        port map (
            bits4  => entrada(11 downto 8),  -- 3º grupo de 4 bits
            valor  => valor_convertido(2)
        );
    
    u_convert3 : entity work.convert4bits_to_fixed
        port map (
            bits4  => entrada(15 downto 12), -- 4º grupo de 4 bits
            valor  => valor_convertido(3)
        );
    
    -- Camada 0 (4 neurônios)
    u_neuronio0 : entity work.neuronio
        port map (
            valor  => valor_convertido,
            peso   => (-0.118292, -0.253516, -0.445719, -0.404179),  -- Pesos neurônio 0
            bias   => (-0.046213),
            saida  => camada0_saida(0)
        );
    
    u_neuronio1 : entity work.neuronio
        port map (
            valor  => valor_convertido,
            peso   => (-0.326143,  0.363044, -0.332116, -0.439777),  -- Pesos neurônio 1
            bias   => (-0.408539),
            saida  => camada0_saida(1)
        );
    
    u_neuronio2 : entity work.neuronio
        port map (
            valor  => valor_convertido,
            peso   => ( 0.360319,  0.050990,  0.351936,  0.383643),  -- Pesos neurônio 2
            bias   => (-1.195297),
            saida  => camada0_saida(2)
        );
    
    u_neuronio3 : entity work.neuronio
        port map (
            valor  => valor_convertido,
            peso   => ( 0.093991, -0.470071,  0.146670,  0.260826),  -- Pesos neurônio 3
            bias   => (1.020519),
            saida  => camada0_saida(3)
        );
    
    -- Camada 1 (3 neurônios)
    u_neuronio_bin1 : entity work.neuronio
        port map (
            valor  => camada0_saida,
            peso   => (-0.284672, -0.465788, 0.344312, -0.153523),  -- Pesos neurônio 0 camada 1
            bias   => (-1.727685),
            saida  => camada1_saida(0)
        );
    
    u_neuronio_bin2 : entity work.neuronio
        port map (
            valor  => camada0_saida,
            peso   => ( 0.110681,  0.098875, -0.379194, -0.466944),  -- Pesos neurônio 1 camada 1
            bias   => (-0.368592),
            saida  => camada1_saida(1)
        );
    
    u_neuronio_bin3 : entity work.neuronio
        port map (
            valor  => camada0_saida,
            peso   => ( 0.008805,  0.338695, 0.370569, -0.258009),  -- Pesos neurônio 2 camada 1
            bias   => (-1.726182),
            saida  => camada1_saida(2)
        );
    
    -- Camada 2 (1 neurônio)
    u_neuronio_final : entity work.neuronio
        port map (
            valor  => camada1_saida,
            peso   => (2.255489, 0.314389, 1.532014),  -- Pesos neurônio camada 2
            bias   => (-2.167732),
            saida  => camada2_saida
        );

    -- Função de ativação binária
    u_neuronio_binario : entity work.neuronio_binario
        port map (
            valor  => camada2_saida,  -- Entrada final para ativação binária
            peso   => "00000000",     -- Pesos irrelevantes para a ativação binária
            bias   => "00000000",     -- Bias irrelevante para a ativação binária
            saida  => saida          -- Resultado final binário
        );

end architecture;
