#include <stdio.h>
#include <cs50.h>
#include <math.h>

int main(void)
{    
    float change_owed;
    // ask user for input
    do
    {
        change_owed = get_float("Change owed: ");
    }
    while (change_owed < 0);
    
    // change dollars to cents
    int cents = round(change_owed * 100);
    
    // calculate amount of coins
    int coins = 0;
    // quarters
    coins += cents / 25;
    cents = cents % 25;
    // dimes
    coins += cents / 10;
    cents = cents % 10;
    // nickels
    coins += cents / 5;
    cents = cents % 5;
    // pennies
    coins += cents / 1;
    
    printf("%i\n", coins);
}
