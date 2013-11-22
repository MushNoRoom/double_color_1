#ifndef LOTTERYMATH_H
#define LOTTERYMATH_H

class LotteryMath
{
public:
    template<typename Iterator>
    static void insertionSort(Iterator begin, Iterator end, bool ascending = false) {
      int i, j;
      int size = end - begin + 1;
      Comp my_comp(ascending);
      for (i = 1; i < size; i++) {
            j = i;
            while (j > 0 && my_comp.compare(*(begin + j - 1), *(begin + j))) {
                auto temp = *(begin + j - 1);
                *(begin + j - 1) = *(begin + j);
                *(begin + j) = temp;
                j--;
            }
      }
    }

private:
    class Comp
    {
    public:
        Comp(bool ascending)
            :m_ascending(ascending)
        {

        }

        template<typename T>
        bool compare(T& i, T &j)
        {
            bool ret_val;
            if (m_ascending)
              i > j ? ret_val = true: ret_val = false;
            else
              i < j ? ret_val = true: ret_val = false;

            return ret_val;
        }
    private:
        bool m_ascending;
    };

};

#endif // LOTTERYMATH_H
