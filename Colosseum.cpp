//
// Created by adire on 30-Nov-17.
//

#include "Colosseum.h"

class Stimulant {
private:
    int stimulant_code;
    int stimulant_factor;
    Gladiator **gladiators1;
    Gladiator **gladiators2;
    int i;
    int j;

    friend class Colosseum;
public:
    Stimulant(int stimulant_code, int stimulant_factor, int num_of_gladiators) : stimulant_code(stimulant_code),
                                                                                 stimulant_factor(stimulant_factor), i(0), j(0){
        gladiators1 = new Gladiator*[num_of_gladiators];
        gladiators2 = new Gladiator*[num_of_gladiators];
    }
    ~Stimulant() {
        delete gladiators1;
        delete gladiators2;
    }
    void operator()(Gladiator gladiator) {
        if(gladiator.id % stimulant_code == 0){
            gladiators1[i] = &gladiator;
            i++;
        } else {
            gladiators2[j] = &gladiator;
            j++;
        }
    }
};

Gladiator::Gladiator(int id, int level) : id(id), level(level) {}

Trainer::Trainer(int id) : id(id) {}

GladiatorID::GladiatorID(int id, int level, Trainer *ptr_to_trainer) : Gladiator(id, level), ptr_to_trainer(ptr_to_trainer) {}

GladiatorID::GladiatorID(int id, int level) : Gladiator(id, level) {}

bool Trainer::operator<(const Trainer &trainer2) const {
    return id < trainer2.id;
}

bool Trainer::operator>(const Trainer &trainer2) const {
    return id > trainer2.id;
}

bool Trainer::operator==(const Trainer &trainer2) const {
    return id == trainer2.id;
}

bool Trainer::operator!=(const Trainer &trainer2) const {
    return !this==trainer2;
}


bool GladiatorID::operator<(const Gladiator &gladiator2) const {
    return id < gladiator2.id;
}

bool GladiatorID::operator>(const Gladiator &gladiator2) const {
    return id > gladiator2.id;
}

bool Gladiator::operator==(const Gladiator &gladiator2) const {
    return id == gladiator2.id;
}

bool Gladiator::operator!=(const Gladiator &gladiator2) const {
    return !this == gladiator2;
}

bool GladiatorLevel::operator<(const Gladiator &gladiator2) const {
    if(level < gladiator2.level) return true;
    if(level == gladiator2.level){
        return id > gladiator2.id;
    }
    return false;
}

bool GladiatorLevel::operator>(const Gladiator &gladiator2) const {
    if(level > gladiator2.level) return true;
    if(level == gladiator2.level){
        return id < gladiator2.id;
    }
    return false;
}

GladiatorLevel::GladiatorLevel(int id, int level) : Gladiator(id, level) {}

Colosseum::Colosseum() : num_gladiators(0), num_trainers(0){}

void Colosseum::addTrainer(int trainer_id) {
    if(trainer_id <= 0){
        throw InvalidParameter();
    }
    trainers_tree.insert(Trainer(trainer_id));
}

void Colosseum::buyGladiator(int gladiator_id, int trainer_id, int level) {
    if(gladiator_id <= 0 || trainer_id <= 0 || level <=0){
        throw InvalidParameter();
    }
    Trainer trainer = trainers_tree.find(Trainer(trainer_id));
    trainer.gladiators.insert(GladiatorLevel(gladiator_id, level));
    gladiators_level_tree.insert(GladiatorLevel(gladiator_id, level));
    gladiators_id_tree.insert(GladiatorID(gladiator_id, level, &trainer));
}

void Colosseum::freeGladiator(int gladiator_id) {
    if(gladiator_id <= 0){
        throw InvalidParameter();
    }
    GladiatorID to_delete = gladiators_id_tree.find(GladiatorID(gladiator_id, 0));
    gladiators_level_tree.remove(GladiatorLevel(gladiator_id, 0));
    Trainer *trainer = to_delete.ptr_to_trainer;
    trainer->gladiators.remove(GladiatorLevel(gladiator_id, 0));
    gladiators_id_tree.remove(to_delete);
}

void Colosseum::levelUp(int gladiator_id, int level_inc) {
    if (gladiator_id <= 0 || level_inc <= 0) {
        throw InvalidParameter();
    }
    GladiatorID gladiator_by_id = gladiators_id_tree.find(GladiatorID(gladiator_id, 0));

    gladiators_level_tree.remove(GladiatorLevel(gladiator_id, 0));
    gladiator_by_id.ptr_to_trainer->gladiators.remove(GladiatorLevel(gladiator_id, 0));
    gladiator_by_id.level += level_inc;

    gladiators_level_tree.insert(GladiatorLevel(gladiator_id, gladiator_by_id.level));
    gladiator_by_id.ptr_to_trainer->gladiators.insert(GladiatorLevel(gladiator_id, gladiator_by_id.level));
}

int Colosseum::getTopGladiator(int trainer_id) {
    if(trainer_id < 0){
        return gladiators_level_tree.getMin().id;
    }
    Trainer trainer = trainers_tree.find(Trainer(trainer_id));
    return trainer.gladiators.getMin().id;
}

void Colosseum::updateGladiator(int gladiator_id, int upgrade_id) {
    if(gladiator_id <= 0 || upgrade_id <= 0){
        throw InvalidParameter();
    }
    GladiatorID to_upgrade = gladiators_id_tree.find(GladiatorID(gladiator_id, 0));
    int level = to_upgrade.level;
    Trainer *trainer = to_upgrade.ptr_to_trainer;
    gladiators_level_tree.remove(GladiatorLevel(gladiator_id, 0));
    gladiators_level_tree.insert(GladiatorLevel(upgrade_id, level));
    trainer->gladiators.remove(GladiatorLevel(gladiator_id, 0));
    trainer->gladiators.insert(GladiatorLevel(upgrade_id, level));
    gladiators_id_tree.remove(to_upgrade);
    gladiators_id_tree.insert(GladiatorID(upgrade_id, level, trainer));
}

static Gladiator** merge(Gladiator** array1, Gladiator** array2, int size){
    int j = 0, k = 0;
    Gladiator **merged_array = new Gladiator*[size];
    for(int i=0; i < size; ++i){
        if(array1[j] < array2[k]){
            merged_array[i] = array1[j];
            j++;
        } else {
            merged_array[i] = array2[k];
            k++;
        }
    }
    return merged_array;
}

void Colosseum::updateLevels(int stimulantCode, int stimulantFactor) {
    if(stimulantCode < 1 || stimulantFactor < 1){
        throw InvalidParameter();
    }
    int size = gladiators_id_tree.getSize();
    Stimulant stimulant(stimulantCode, stimulantFactor, size);
    gladiators_id_tree.inOrder(stimulant);
    delete gladiators_id_tree;
    Gladiator** merged_array = merge(stimulant.gladiators1, stimulant.gladiators2, size);
    for (int i = 0; i < size; ++i) {
        GladiatorID *gladiatorID = merged_array[i];
        gladiators_id_tree.insert(merged_array)
    }
    //and make the new constructor to rebuild the tree with the new merged array
    //do the same for all the trees
}





