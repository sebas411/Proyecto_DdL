#include "lib.hpp"

DFA minimize(DFA automatonA) {
    DFA automatonB;
    
    vector<Group> partition;

    // create initial partition with 2 groups, accepting and non accepting states
    partition.push_back(Group(Substraction(automatonA.states, automatonA.final_states)));
    partition.push_back(Group(automatonA.final_states));

    while (true) {
        vector<Group> new_partition;
        // initially, let partnew = part
        // for ( each group G of part )
        for (Group G: partition) {
            vector<Group> subgroups;
            // check every state in the group
            for (State s: G.states) {
                set<GroupTransition> s_transitions;
                //create a grouptransition for every symbol sym
                for (Symbol sym: automatonA.symbols) {
                    State dest = automatonA.move(s, sym);
                    int dest_group = -1;
                    //find destiny group for symbol sym
                    for (int i = 0; i < partition.size(); i++) {
                        if (partition[i].states.find(dest) != partition[i].states.end()) {
                            dest_group = i;
                            break;
                        }
                    }
                    GroupTransition sym_transition = GroupTransition(sym, dest_group);
                    s_transitions.insert(sym_transition);
                }
                bool in_subgroup = false;
                // check if state s belongs to an existing group and if it does add the state to the group
                for (vector<Group>::iterator it = subgroups.begin(); it != subgroups.end(); it++) {
                    Group & subgroup = *it;
                    if (subgroup.transitions == s_transitions) {
                        subgroup.states.insert(s);
                        in_subgroup = true;
                        break;
                    }
                }
                // if it is not found create a new group with it and add it to subgroups of group B
                if (!in_subgroup) {
                    Group new_group = Group(s_transitions, s);
                    subgroups.push_back(new_group);
                }
            }
            // add all subgroups of group G to new_partition
            for (Group subgroup: subgroups) {
                new_partition.push_back(subgroup);
            }
        }
        // check if new_partition is equal to partition
        bool same = true;
        for (int i = 0; i < partition.size(); i++) {
            if (partition[i].states != new_partition[i].states) {
                same = false;
            }
        }
        partition = new_partition;
        if (same) break;
    }
    bool was_initial_state_found = false;
    State representatives[partition.size()];
    // set states, initial_state and final_states
    for (int i = 0; i < partition.size(); i++) {
        Group G = partition[i];
        bool is_representative_set = false;
        bool has_acceptance_state = false;
        for (State s: G.states) {
            if (!is_representative_set) {
                representatives[i] = s;
                is_representative_set = true;
            }
            if (automatonA.initial_state == s) {
                was_initial_state_found = true;
                automatonB.initial_state = representatives[i];
            }
            if (automatonA.final_states.find(s) != automatonA.final_states.end()) {
                has_acceptance_state = true;
                if (was_initial_state_found) break;
            }
        }
        automatonB.states.insert(representatives[i]);
        if (has_acceptance_state) automatonB.final_states.insert(representatives[i]);
    }
    // re-iterate over partition to set transitions now that representatives is set
    for (int i = 0; i < partition.size(); i++) {
        Group G = partition[i];
        for (GroupTransition gt: G.transitions) {
            automatonB.transitions.push_back(Transition(representatives[i], representatives[gt.destGroup], gt.symbol));
        }
    }
    return automatonB;
}