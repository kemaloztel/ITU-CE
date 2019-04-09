/*
 * Author: Kadir Emre Oto
 * Student ID: 150140032
 * Assignment: 3
 * Lecture: Object Oriented Programming (BLG252E)
 */

#include "icecream.h"

Icecream::Icecream(string pi, double pc, double pr){
    name = pi;
    amount = pc;
    price = pr;
}

std::ostream& operator<< (std::ostream& os, const Icecream& it){
    os << it.name << " #" << it.amount << " Cost: " << it.price * it.amount;
    return os;
}

string Icecream::get_name() const{
    return name;
}

double Icecream::get_amount() const{
    return amount;
}

double Icecream::get_price() const{
    return price;
}

double Icecream::get_cost() const{
    return price * amount;
}