/*
The KV Cup is the online programming competition for pairs of programmers.
Coach of some big university is in charge of registration of teams for this competition. There is an even number of students who desire to compete, so each student does compete.
For each student the rating on the famous site Forcedcoders is known. Coach considers rating of the pair as sum of ratings of the contestants in it. Coach wants to form teams in such a way, that lowest rating of the pair is maximized. Help him to find this rating.
Note that each student may participate exactly in one pair.
Input
The first line of input contains a single integer n (1 ≤ n ≤ 105), representing the number of students who desire to enter the KV Cup. It is guaranteed that n is an even number. Each of the following n lines contains a single integer r (1 ≤ r ≤ 106), representing the rating of a student.
Output
Print one integer — maximal possible value of lowest rating of the pair.
*/

#include <iostream>
#include <vector>
#include <algorithm>
int main() {
    int n;
    std::cin >> n;
    std::vector<long> a;
    long b;
    for (int i = 0; i < n; i++) {
        std::cin >> b;
        a.push_back(b);
    }
    sort(a.begin(), a.end());
    std::cout <<std::endl;
    std::vector<long> pairs;
    for (int i = 0; i < n/2; i++) {
        b = a[i] + a[n - i - 1];
        pairs.push_back(b);
    }
    sort(pairs.begin(), pairs.end());
    std::cout << pairs[0] << std::endl;
    return 0;
}
