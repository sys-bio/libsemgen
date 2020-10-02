//
// Created by Ciaran on 4/22/2020.
//

#include "omexmeta/PhysicalForce.h"


namespace omexmeta {

    PhysicalForce::PhysicalForce(librdf_model *model, std::string model_uri, std::string local_uri,
                                 PhysicalProperty physicalProperty,
                                 Sources sources,
                                 Sinks sinks)
            : PhysicalPhenomenon(model, model_uri, local_uri, std::move(physicalProperty), PHYSICAL_PROCESS),
              sources_(std::move(sources)), sinks_(std::move(sinks)) {

    }

    const std::vector<SourceParticipant> &PhysicalForce::getSources() const {
        return sources_;
    }

    const std::vector<SinkParticipant> &PhysicalForce::getSinks() const {
        return sinks_;
    }

    std::string PhysicalForce::createMetaId() {
        return generateMetaId("PhysicalForce");
    }

    Triples PhysicalForce::toTriples() {
        if (getAbout().empty()) {
            throw AnnotationBuilderException(
                    "PhysicalForce::toTriples(): Cannot create"
                    " triples because the \"about\" information is not set. "
                    "Use the setAbout() method."
            );
        }
        if (getPhysicalProperty().getResourceStr().empty()) {
            throw AnnotationBuilderException(
                    "PhysicalProcess::toTriples(): cannot create "
                    "triples object because the\"location\" information "
                    "is empty. Please use the \"hasProperty()\"."
            );

        }
        if (physical_force_id_.empty()){
            physical_force_id_ = generateMetaId("PhysicalForce");
        }
        physical_force_id_ = OmexMetaUtils::concatMetaIdAndUri(physical_force_id_, getLocalUri());

        Triples triples = physical_property_.toTriples(physical_force_id_);
        for (auto &source : sources_) {
            for (auto &triple : source.toTriples(physical_force_id_, new_metaid_exclusion_list_)) {
                triples.move_back(triple);
            }
        }
        for (auto &sink : sinks_) {
            for (auto &triple : sink.toTriples(physical_force_id_, new_metaid_exclusion_list_)) {
                triples.move_back(triple);
            }
        }
        return triples;
    }

    PhysicalForce &PhysicalForce::setPhysicalProperty(PhysicalProperty physicalProperty) {
        physical_property_ = std::move(physicalProperty);
        return (*this);
    }

    PhysicalForce &PhysicalForce::setPhysicalProperty(std::string subject_metaid, std::string physical_property) {
        if (!OmexMetaUtils::startsWith(subject_metaid, "http")) {
            subject_metaid = OmexMetaUtils::concatMetaIdAndUri(subject_metaid, getModelUri());
        }
        physical_property_ = PhysicalProperty(std::move(subject_metaid), std::move(physical_property), getModelUri());
        return *this;
    }

    PhysicalForce &PhysicalForce::addSource(int multiplier, const std::string &physical_entity_reference) {
        sources_.push_back(
                std::move(SourceParticipant(
                        model_, multiplier, physical_entity_reference, getModelUri(), getLocalUri()
                ))
        );
        return (*this);
    }

    PhysicalForce &PhysicalForce::addSink(int multiplier, const std::string &physical_entity_reference) {
        sinks_.push_back(
                SinkParticipant(model_, multiplier, physical_entity_reference, getModelUri(), getLocalUri())
        );
        return (*this);
    }

    PhysicalForce::PhysicalForce(librdf_model *model)
            : PhysicalPhenomenon(model) {}

    PhysicalForce::PhysicalForce(librdf_model *model, const std::string &model_uri, const std::string &local_uri)
            : PhysicalPhenomenon(model, model_uri, local_uri) {}

    int PhysicalForce::getNumSources() {
        return sources_.size();
    }

    int PhysicalForce::getNumSinks() {
        return sinks_.size();
    }

    void PhysicalForce::free() {

        for (auto &i : sources_) {
            i.free();
        }
        for (auto &i : sinks_) {
            i.free();
        }
    }

    bool PhysicalForce::operator==(const PhysicalForce &rhs) const {
        return static_cast<const omexmeta::PhysicalPhenomenon &>(*this) ==
               static_cast<const omexmeta::PhysicalPhenomenon &>(rhs) &&
               sources_ == rhs.sources_ &&
               sinks_ == rhs.sinks_;
    }

    bool PhysicalForce::operator!=(const PhysicalForce &rhs) const {
        return !(rhs == *this);
    }

    PhysicalForce &PhysicalForce::hasProperty(const std::string &property) {
        physical_property_.setResource(property);
        return *this;
    }


    PhysicalForce &PhysicalForce::setAbout(const std::string &about) {
        physical_property_.setSubject(about);
        return *this;
    }

}