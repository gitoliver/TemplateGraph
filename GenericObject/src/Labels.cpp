#include "../includes/Labels.hpp"

namespace Abstract
{

/************************************************
 *  CONSTRUCTORS/DESTRUCTORS
 ***********************************************/
Labels::Labels()
{
	this->setName("INCORRECT LABEL CONSTRUCTOR");
}

Labels::Labels(std::string name)
{
	this->setName(name);
}

Labels::Labels(std::vector<std::string> labels)
{
	this->setLabels(labels);
}

Labels::Labels(std::string name, std::string label)
{
	this->setName(name);
	this->addLabel(label);
}

Labels::Labels(std::string name, std::vector<std::string> labels)
{
	this->setName(name);
	this->setLabels(labels);
}

Labels::~Labels()
{
}

/************************************************
 *  GETTER/SETTER
 ***********************************************/

std::string Labels::getName() const
{
	return this->name;
}

std::string Labels::getLabel() const
{
	if (this->labels.empty())
		return "";
	return this->labels.back();
}

std::vector<std::string> Labels::getLabels() const
{
	return this->labels;
}

void Labels::setName(std::string name)
{
	this->name = name;
}

void Labels::setLabels(std::string label)
{
	this->labels.clear();
	this->addLabel(label);
}

void Labels::setLabels(std::vector<std::string> labels)
{
	this->labels = labels;
}

/************************************************
 *  MUTATORS
 ***********************************************/

void Labels::addLabel(std::string label)
{
	this->labels.push_back(label);
}

void Labels::addLabels(std::vector<std::string> labels)
{
	this->labels.insert(this->labels.end(), labels.begin(), labels.end());
}

/************************************************
 *  FUNCTIONS
 ***********************************************/

bool Labels::containsLabel(const std::string query)
{
	for (std::string currLabel : this->labels)
	{
		//TODO: Why was regex used in your version?
		if (currLabel == query)
			return true;
	}
	return false;
}

bool Labels::compareLabels(const std::vector<std::string> otherLabels)
{
	return this->compareLabels(otherLabels, 1);
}

bool Labels::compareLabels(const std::vector<std::string> otherLabels,
		unsigned int numMatches)
{
	unsigned int matchCounter = 0;
	for (std::string currLabel : this->labels)
	{
		if (std::find(otherLabels.begin(), otherLabels.end(), currLabel)
				!= otherLabels.end())
		{
			matchCounter++;
			if (matchCounter == numMatches)
				return true;
		}
	}
	return false;
}

}
