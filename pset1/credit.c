#include <stdio.h>
#include <cs50.h>

int main(void)
{
    long number = get_long("Number: ");
    int number_lenght = 0;
    int digit;
    int first_two_digits;
    int first_digit;
    int odd_sum = 0;
    int even_sum = 0;
    while (number != 0)
    {
        number_lenght++;
        digit = number % 10;
        if (number_lenght % 2 == 0)
        {
            digit *= 2;
            do
            {
                even_sum += digit % 10;
                digit /= 10;
            }
            while (digit != 0);
        }
        else
        {
            odd_sum += digit;
        }
        if (number < 100 && number >= 10)
        {
            first_two_digits = number;
        }
        else if (number < 10)
        {
            first_digit = number;
        }
        number /= 10;
    }
    if ((even_sum + odd_sum) % 10 == 0)
    {
        if (number_lenght == 15 && (first_two_digits == 34 || first_two_digits == 37))
        {
            printf("AMEX\n");
        }
        else if (number_lenght == 16 && (first_two_digits == 51 || first_two_digits == 52 
                                         || first_two_digits == 53 || first_two_digits == 54 
                                         || first_two_digits == 55))
        {
            printf("MASTERCARD\n");
        }
        else if ((number_lenght == 13 || number_lenght == 16) && first_digit == 4)
        {
            printf("VISA\n");
        }
        else
        {
            printf("INVALID\n");
        }
    }
    else
    {
        printf("INVALID\n");
    }
}
