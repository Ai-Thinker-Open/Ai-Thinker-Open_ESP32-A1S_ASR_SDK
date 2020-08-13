set(COMPONENT_SRCDIRS 
    ./MediaHal
    ./MediaHal/Codec
    ./SystemSal
    ./i2c_bus
    ./ac101_driver
    )

set(COMPONENT_ADD_INCLUDEDIRS 
    ./MediaHal/Board
    ./MediaHal/Codec
    ./MediaHal/
    ./SystemSal
    ./i2c_bus
    ./userconfig
    ./ac101_driver
    )


register_component()


