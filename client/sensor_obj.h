struct lineSensor {
    int index;
    float blank_lower;
    float blank_upper;
    float line_lower;
    float line_upper;
    float led_lower;
    float led_upper;
};

lineSensor line_sensors[8] = {
    {0, 17.0, 26.0, 1.0, 3.0, 4.0, 8.0},
    {1, 37.0, 53.0, 1.0, 8.0, 0.0, 0.0},
    {2, 37.0, 45.0, 82.0, 90.0, 73.0, 80.0},
    {3, 43.0, 52.0, 1.0, 4.0, 9.0, 14.0},
    {4, 8.0, 13.0, 1.0, 2.0, 0.0, 0.0},
    {5, 3.0, 11.0, 1.0, 1.0, 0.0, 0.0},
    {6, 8.0, 22.0, 1.0, 1.0, 4.0, 8.0},
    {7, 8.0, 38.0, 1.0, 1.0, 2.0, 3.0}
};
