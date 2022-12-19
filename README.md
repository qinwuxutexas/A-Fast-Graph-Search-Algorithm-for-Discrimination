# Multibranch depth first search graph tree with dynamic & constrained optimization algorithm, for finding the best classification rule(s). 

This algorithm aims to find the best path/rule for making classification decision. It is different than the general tree algorithm as:
1) the general tree algorithm only picks out one "best" node at each path and depth as a root node and then goes down to left or right as a binary tree until reaching the leaf at the end. In the inference, the decision rule (path) may vary dependent on the input feature data.
2) This algoirthm targets to find the best classification rules with feature ranking. For example, the green line path in the below graph is the best classification rule identified and to be used in classification decision. 
3) At each depth of each path, three features/nodes are picked out using the top k algorithm (time complexity: nlog(K)) instead of only one as general decision tree does. This increases the likelihood of performance since some lower-rank nodes at upper level may actually result in better performance at lower levels when going down to the leaf along the path, but the general decision tree may miss that opportunity by only picking out the best node at each iteration.

Ojbective function: L = min (alpha*Dleta(TP) - Delta(FP)), where Dleta(TP) is the change of true positive rate and Delta (FP) is the change of false positive rate, and alpha is a weight parameter. The FP is constrained to <= threshold.

1) features are ranked at each depth level, and 3 features are selected using the top k algorithm according its objective function value.
2) graph is built based on the depth-first-serch algorithm from top to leaf. It looks like a n-ary tree, but I used the graph to store the nodes and rules, and therefore, I call it a "multi-branch graph".
3) the best path out of all paths (e.g., the green line path as an exmple here) is identified based on the objective function.
4) Dyanmic optimization is evolving as: at the upper layers (smaller depth) the optimization focus more on the TP as it strives to obtain the highest TP as possible, while at the lower levels (greater deapth) the optimization gradually reduces FP rates given the constrained condition.

![image](https://user-images.githubusercontent.com/78186650/208330987-083eedf2-cf54-49bc-83ad-7542d04cc889.png)



![image](https://user-images.githubusercontent.com/78186650/208331194-2e7ea1b9-69d5-4a69-80cd-42869c0d439a.png)

![image](https://user-images.githubusercontent.com/78186650/208331214-b9ce50b8-aa61-44ba-a731-226e26e268b6.png)



