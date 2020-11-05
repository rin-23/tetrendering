#include "muscle_app.h"

int main(int argc, char **argv) 
{
    tyro::App appr;
    //appr.Setup(1920, 1020, false);
    int r_code = appr.Launch();
    return 0;
}
