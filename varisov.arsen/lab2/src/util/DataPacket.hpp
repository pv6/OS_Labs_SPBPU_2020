#ifndef LAB2_DATA_PACKET_HPP
#define LAB2_DATA_PACKET_HPP

const int uninitializedTemp = -1000;

struct DataPacket
{
    DataPacket(int day_, int month_, int year_, bool isLastPacket_) : day(day_)
                                                                    , month(month_)
                                                                    , year(year_)
                                                                    , isLastPacket(isLastPacket_)
                                                                    , temperature(uninitializedTemp)
                                                                    {};

    DataPacket() : temperature(uninitializedTemp) {};

    int         day;
    int         month;
    int         year;
    bool        isLastPacket;         
    int         temperature;
};

#endif //LAB2_DATA_PACKET_HPP
