#include <iostream>
#include <vector>

int main()
{
    std::vector<int> myVector;
    std::cout << "Size: " << myVector.size() << " Capacity: " << myVector.capacity() << std::endl;

    myVector.shrink_to_fit();
    std::cout << "Size: " << myVector.size() << " Capacity: " << myVector.capacity() << std::endl;

    std::cout << "for\n\n";
    // Add some elements to the vector
    for (int i = 0; i < 16; i++)
    {
        myVector.push_back(i);
        std::cout << "Size: " << myVector.size() << " Capacity: " << myVector.capacity() << std::endl;

        /*
        myVector.shrink_to_fit();

        std::cout << "Size: " << myVector.size() << " Capacity: " << myVector.capacity() << std::endl;
        */
       
        if (i == 12)
        {
            myVector.shrink_to_fit();

            std::cout << "12 Size: " << myVector.size() << " Capacity: " << myVector.capacity() << std::endl;

            myVector.clear();

            std::cout << "12 Size: " << myVector.size() << " Capacity: " << myVector.capacity() << std::endl;

            myVector.shrink_to_fit();

            std::cout << "12 Size: " << myVector.size() << " Capacity: " << myVector.capacity() << std::endl;
        }

        std::cout << "------------------------\n";
    }

    return 0;
}
