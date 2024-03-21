# 6 Main Topics

1. Object Dictionary (OD)
        Basically the database.
        Lots of objects in it. (e.g., gamepadin, gamepadout)
        Purposed for (a) config, (b) transfer, (c) receive or (d) monitor
2. Service Data Objects (SDO)
        Client (master) can request to read/write to OD.
        Server (slave) fulfills these request.
        Generally async (client can send it anytime).
3. Process Data Objects (PDO)
        Producer (master/slave) transmit small & periodic data.
        Consumer (master/slave) receive the data.
        The data is just an object in the OD.
        Nodes agree before-hand on which object is transfer/receive.
                This is also known as PDO mapping. (language specific)
4. Node-ID
        Built on top of CAN bus ID system (11 bit).
        Each node in the network has an ID (7 bit).
        Rest is used for function codes (4 bit). 
        Combination of function code + ID is called COB-ID (11 bit).

5. Network Management (NMT)
        Additional protocol within CANopen.
        Used to configure & monitor the network.
        One node acts as the master (controls).
        Each node has a state
                (a) initial
                (b) pre-operational
                (c) operational
                (d) stopped
        Master sends NMT messages to other nodes (change state).
        Monitoring happens via heartbeats. (e.g., r u good @ 100ms)

6. Device Profiles
        CANopen specification defines device profiles (e.g., I/O, motion, lifts)
        Basically standardization of objects in the OD.
                Structure, variables, attributes, types.
                Which are TPDO and which are RPDO.
        Some examples:
                CiA 401: Generic I/O modules
                CiA 402: Drives and Motion Control
                CiA 417: Lift Control Systems

