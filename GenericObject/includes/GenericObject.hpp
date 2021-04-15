#ifndef GENERIC_OBJECT_HPP
#define GENERIC_OBJECT_HPP

#include "Labels.hpp"

namespace Abstract
{
/* TODO: Figure out a better name and what data we want to include. Do
 * 			we want/need index counting? This could be a pain when
 * 			deletions and insertions come into play. All my algos (except
 * 			subgraph matching) ignore these signifiers.
 * 			We could name it "generic identifiers" or something of the
 * 			sort.
 */
class GenericObject
{
public:
	/************************************************
	 *  CONSTRUCTORS/DESTRUCTORS
	 ***********************************************/
	GenericObject()
	{
	}

	GenericObject(std::string name)
	{
		this->setName(name);
	}

	GenericObject(std::string name, std::string label)
	{
		this->setName(name);
		this->setLabels(label);
	}
	GenericObject(std::string name, std::vector<std::string> labels)
	{
		this->setName(name);
		this->setLabels(labels);
	}
	~GenericObject()
	{
		//lazyInfo(__LINE__, __func__, "Destructing generic object for name <"+this->getName()+">");
	}

	/************************************************
	 *  GETTER/SETTER
	 ***********************************************/
	std::string getName()
	{
		return this->labels.getName();
	}
	std::string getLabel()
	{
		return this->labels.getLabel();
	}
	std::vector<std::string> getLabels()
	{
		return this->labels.getLabels();
	}

	void setName(std::string name)
	{
		this->labels.setName(name);
	}

	void setLabels(std::string label)
	{
		this->labels.setLabels(label);
	}
	void setLabels(std::vector<std::string> labels)
	{
		this->labels.setLabels(labels);
	}

	/************************************************
	 *  MUTATORS
	 ***********************************************/
	void addLabel(std::string label)
	{
		this->labels.addLabel(label);
	}

	/************************************************
	 *  FUNCTIONS
	 ***********************************************/
	bool compareLabels(const std::vector<std::string> otherLabels);

private:
	/************************************************
	 *  ATTRIBUTES
	 ***********************************************/
	Labels labels;

};
}
#endif // GENERIC_OBJECT_HPP
