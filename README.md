# Multibranch depth first search graph tree with dynamic & constrained optimization algorithm, for finding the best classification rule(s). 

This proposed algorithm aims to find the best path/rule for making classification decision. It is different than the general binary search tree (bst) algorithm as:
1) the bst algorithm only picks out one "best" node at each path and depth as a root node and then goes down to left or right until reaching the leaf at the end. When predict results using the trained bst model, the decision rule (path) may vary dependent on the input feature data.
2) This proposed algoirthm targets to find the ranked classification rules by using multi-branch feature ranking. For example, the green line path in the below graph is the best classification rule identified from the model train. Users may pick out ranked path(s) of rules to make classification decision in prediction.
3) At each depth of each path, three features/nodes are picked out using the top-k ranking algorithm instead of only one node as the binary search tree does. The rational of doing this way is to increases the likelihood of performance since some lower-ranked nodes at upper level may actually lead to higher performance toward the lower levels and the leaf along a path. In comparision, the bst could miss some opportunity by only picking out the best node at each "iteration".

Ojbective function: L = min (alpha*Dleta(TP) - Delta(FP)), where Dleta(TP) is the change of true positive rate and Delta (FP) is the change of false positive rate, and alpha is a weight parameter. The FP is constrained to <= threshold.

1) features are ranked at each depth level, and the top-3 ranked features are selected according its objective function value.
2) graph is built based on the depth-first-serch algorithm from the root to the leaf. It looks like a n-ary tree, but I used graph to store the nodes and identified rules, and therefore, I call it a "multi-branch graph".
3) paths are ranked according to its performance of objectives, e.g., the best path is highlighed as a green one as an example. I would review the top three-five ranked paths to compare their performances.
4) Dyanmic optimization is used as an evolving process as follows: 
at the upper layers (smaller depth) the optimization focus more on the TP as it strives to obtain the highest TP as possible, while at the lower levels (greater deapth) the optimization gradually reduces FP rates given the constrained condition of FP.

![image](https://user-images.githubusercontent.com/78186650/208330987-083eedf2-cf54-49bc-83ad-7542d04cc889.png)



![image](https://user-images.githubusercontent.com/78186650/208331194-2e7ea1b9-69d5-4a69-80cd-42869c0d439a.png)

![image](https://user-images.githubusercontent.com/78186650/208331214-b9ce50b8-aa61-44ba-a731-226e26e268b6.png)



