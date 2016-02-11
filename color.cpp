namespace color
{

Color random()
{
    return Color { rand()%255/255.0f, rand()%255/255.0f, rand()%255/255.0f, 1.0f };
}

} // namesapce color
