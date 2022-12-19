# multibranch_depth_first_search_tree with dynamic & constrained optimization algorithm

Ojbective function: min(alpha*TP - FP), where TP is true positive and FP is false positive, and alpha is a weight parameter. The FP is constrained to <= threshold.
1) features are ranked at each depth level, and picked out of 3 features using the top k algorithm.
2) graph is built based on the depth-first-serch algorithm from top to leaf
3) the best path out of all paths (e.g., the green line path as an exmple here) is identified based on the objective function.
4) Dyanmic optimization is evolving as: at the upper layers (smaller depth) the optimization focus more on the TP as it strives to obtain the highest TP as possible, while at the lower levels (greater deapth) the optimization gradually reduces FP rates given the constrained condition.

![image](https://user-images.githubusercontent.com/78186650/208330987-083eedf2-cf54-49bc-83ad-7542d04cc889.png)



![image](https://user-images.githubusercontent.com/78186650/208331194-2e7ea1b9-69d5-4a69-80cd-42869c0d439a.png)

![image](https://user-images.githubusercontent.com/78186650/208331214-b9ce50b8-aa61-44ba-a731-226e26e268b6.png)



