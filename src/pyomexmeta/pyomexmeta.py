from __future__ import annotations

import ctypes as ct
import os
from contextlib import contextmanager
from typing import List

from .pyomexmeta_api import PyOmexMetaAPI


def _xml_type_factory(xml_type: str):
    """
    Helper function used internally for directing the underlying c++
    code to work with cellml or sbml.
    """
    type = None
    if xml_type == "sbml":
        type = 0
    elif xml_type == "cellml":
        type = 1
    else:
        raise ValueError("\"xml_type\" arg must be either \"sbml\" or \"cellml\"")
    return type


class RDF:

    def __init__(self, storage_type: str = "hashes", storage_name: str = "semsim_storage", storage_options: str = None,
                 model_options: str = None, rdf_ptr: ct.c_int64 = None):
        # when pointer argument not given by user, create new instance of RDF
        # argument is only given manually when static methods are used and
        # this is hidden from users.
        if not rdf_ptr:
            self._obj = PyOmexMetaAPI.rdf_new(
                storage_type.encode(), storage_name.encode(),
                None if storage_options is None else storage_options.encode(),
                None if model_options is None else model_options.encode(),
            )
        else:
            self._obj = rdf_ptr

    def __len__(self):
        """Returns the number of individual Triples stored in the rdf model"""
        return PyOmexMetaAPI.rdf_size(self._obj)

    def __str__(self):
        """Defaults to rdfxml-abbrev syntax"""
        return self.to_string("rdfxml-abbrev")

    def __del__(self):
        """deletes the RDF instance"""
        self.delete()

    def _set_rdf_ptr(self, ptr: ct.c_int64):
        """
        Change the RDF pointer to ptr
        Args:
            ptr: a pointer to the C generated RDF object.

        Returns:

        """
        # first remove the existing pointer
        self.delete()
        # then do the switch
        self._obj = ptr

    @staticmethod
    def from_string(rdf_string: str, format: str = "guess", base_uri: str = "./Annotations.rdf",
                    storage_type: str = "hashes", storage_name: str = "semsim_storage", storage_options: str = None,
                    model_options: str = None) -> RDF:
        """read rdf from a string"""
        rdf_ptr = PyOmexMetaAPI.rdf_from_string(
            rdf_string.encode(), format.encode(), base_uri.encode(),
            storage_type.encode(), storage_name.encode(),
            None if not storage_options else storage_options.encode(),
            None if not storage_options else model_options.encode()
        )
        rdf = RDF()
        rdf._set_rdf_ptr(rdf_ptr)
        return rdf

    def add_from_string(self, rdf_string: str, format: str = "guess", base_uri: str = "./Annotations.rdf") -> None:
        PyOmexMetaAPI.rdf_add_from_string(self._obj, rdf_string.encode(), format.encode(), base_uri.encode())

    @staticmethod
    def from_uri(uri_string: str, format: str, storage_type: str = "hashes", storage_name: str = "semsim_storage",
                 storage_options: str = None,
                 model_options: str = None) -> RDF:
        rdf_ptr = PyOmexMetaAPI.rdf_from_uri(
            uri_string.encode(), format.encode(),
            storage_type.encode(), storage_name.encode(),
            None if not storage_options else storage_options.encode(),
            None if not storage_options else model_options.encode()
        )
        rdf = RDF()
        rdf._set_rdf_ptr(rdf_ptr)
        return rdf

    def add_from_uri(self, uri_string: str, format: str) -> None:
        PyOmexMetaAPI.rdf_add_from_uri(self._obj, uri_string.encode(), format.encode())

    @staticmethod
    def from_file(filename: str, format: str, storage_type: str = "hashes", storage_name: str = "semsim_storage",
                  storage_options: str = None, model_options: str = None) -> RDF:
        rdf_ptr = PyOmexMetaAPI.rdf_from_file(
            filename.encode(), format.encode(),
            storage_type.encode(), storage_name.encode(),
            None if not storage_options else storage_options.encode(),
            None if not storage_options else model_options.encode()
        )
        rdf = RDF()
        rdf._set_rdf_ptr(rdf_ptr)
        return rdf

    def add_from_file(self, filename: str, format: str) -> None:
        PyOmexMetaAPI.rdf_add_from_file(self._obj, filename.encode(), format.encode())

    def delete(self) -> None:
        """destructor. Delete the dynamically allocated rdf object"""
        PyOmexMetaAPI.rdf_delete(self._obj)

    def to_string(self, format: str, base_uri: str = "./Annotations.rdf") -> str:
        str_ptr = PyOmexMetaAPI.rdf_to_string(self._obj, format.encode(), base_uri.encode())
        thestring = PyOmexMetaAPI.get_and_free_c_str(str_ptr)
        return thestring

    def get_base_uri(self) -> str:
        return PyOmexMetaAPI.get_and_free_c_str(PyOmexMetaAPI.rdf_get_base_uri(self._obj))

    def set_base_uri(self, uri: str) -> None:
        if not os.path.isfile(uri):
            uri = os.path.join(os.path.realpath(os.getcwd()), uri)
        PyOmexMetaAPI.rdf_set_base_uri(self._obj, uri.encode())

    def query(self, query_str: str, results_format: str) -> str:
        results_crlf = PyOmexMetaAPI.get_and_free_c_str(
            PyOmexMetaAPI.rdf_query_results_as_str(
                self._obj, query_str.encode(), results_format.encode())
        )
        results_lf = PyOmexMetaAPI.crlf_to_lr(results_crlf)
        return results_lf

    def to_editor(self, xml: str, xmltype: str) -> Editor:
        return Editor(PyOmexMetaAPI.rdf_to_editor(self._obj, xml.encode(),
                                                _xml_type_factory(xml_type=xmltype)))

    def draw(self, filename: str):
        """
        render an graph of RDF and save to `filename`

        Args:
            filename: where to write. The extension determines the format. See
            https://graphviz.readthedocs.io/en/stable/index.html for more
            details about accepted formats.

        Returns:

        """
        try:
            import graphviz
        except ImportError:
            raise ImportError('"graphviz" not found. Install '
                              'with "sudo apt install graphviz" and then '
                              '"pip install graphviz"')
        dot = self.to_string("dot", "annotation-drawing.rdf")
        src = graphviz.Source(dot)
        src.render(filename)
        print('RDF graph saved to "{}"'.format(filename))
        if not os.path.isfile(filename):
            raise ValueError("Output was not written to file \"{}\"".format(filename))


class Editor:

    def __init__(self, editor_ptr: ct.c_int64):
        self._obj = editor_ptr

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.delete()

    def add_namespace(self, namespace: str, prefix: str) -> None:
        PyOmexMetaAPI.editor_add_namespace(self._obj, namespace, prefix)

    def add_singular_annotation(self, singular_annotation: SingularAnnotation) -> None:
        PyOmexMetaAPI.editor_add_single_annotation(self._obj, singular_annotation.get_ptr())

    def add_physical_entity(self, physical_entity: PhysicalEntity) -> None:
        PyOmexMetaAPI.editor_add_physical_entity(self._obj, physical_entity.get_ptr())

    def add_physical_process(self, physical_process: PhysicalProcess) -> None:
        PyOmexMetaAPI.editor_add_physical_process(self._obj, physical_process.get_ptr())

    def add_physical_force(self, physical_force: PhysicalForce) -> None:
        PyOmexMetaAPI.editor_add_physical_force(self._obj, physical_force.get_ptr())

    def check_valid_metaid(self, id: str) -> None:
        PyOmexMetaAPI.editor_check_valid_metaid(self._obj, id)

    def get_metaids(self) -> List[str]:
        num_ids = PyOmexMetaAPI.editor_get_num_metaids(self._obj)
        return [PyOmexMetaAPI.get_and_free_c_str(
            PyOmexMetaAPI.editor_get_metaid(self._obj, id)
        ) for id in range(num_ids)]

    def get_xml(self) -> str:
        return PyOmexMetaAPI.get_and_free_c_str(
            PyOmexMetaAPI.editor_get_xml(self._obj)
        )

    @contextmanager
    def new_singular_annotation(self) -> SingularAnnotation:
        singular_annotation = SingularAnnotation(PyOmexMetaAPI.editor_new_singular_annotation(self._obj))
        try:
            yield singular_annotation
        finally:
            self.add_singular_annotation(singular_annotation)

    @contextmanager
    def new_physical_entity(self) -> PhysicalEntity:
        physical_entity = PhysicalEntity(PyOmexMetaAPI.editor_new_physical_entity(self._obj))
        try:
            yield physical_entity
        finally:
            self.add_physical_entity(physical_entity)

    @contextmanager
    def new_physical_process(self) -> PhysicalProcess:
        physical_process = PhysicalProcess(PyOmexMetaAPI.editor_new_physical_process(self._obj))
        try:
            yield physical_process
        finally:
            self.add_physical_process(physical_process)

    @contextmanager
    def new_physical_force(self) -> PhysicalForce:
        physical_force = PhysicalForce(PyOmexMetaAPI.editor_new_physical_force(self._obj))
        try:
            yield physical_force
        finally:
            self.add_physical_force(physical_force)

    def delete(self):
        PyOmexMetaAPI.editor_delete(self._obj)


class SingularAnnotation:

    def __init__(self, singular_annotation_ptr: ct.c_int64):
        self._obj = singular_annotation_ptr

    def get_ptr(self) -> ct.c_int64:
        return self._obj

    def set_about(self, about: str) -> SingularAnnotation:
        self._obj = PyOmexMetaAPI.singular_annotation_set_about(self._obj, about.encode())
        return self

    def set_predicate(self, namespace: str, term: str) -> SingularAnnotation:
        self._obj = PyOmexMetaAPI.singular_annotation_set_predicate(self._obj, namespace.encode(), term.encode())
        return self

    def set_predicate_uri(self, uri: str) -> SingularAnnotation:
        self._obj = PyOmexMetaAPI.singular_annotation_set_predicate_uri(self._obj, uri.encode())
        return self

    def set_resource_literal(self, literal: str) -> SingularAnnotation:
        self._obj = PyOmexMetaAPI.singular_annotation_set_resource_literal(self._obj, literal.encode())
        return self

    def set_resource_uri(self, uri: str) -> SingularAnnotation:
        self._obj = PyOmexMetaAPI.singular_annotation_set_resource_uri(self._obj, uri.encode())
        return self

    def set_resource_blank(self, blank_id: str) -> SingularAnnotation:
        self._obj = PyOmexMetaAPI.singular_annotation_set_resource_blank(self._obj, blank_id.encode())
        return self

    def get_about(self) -> str:
        return PyOmexMetaAPI.get_and_free_c_str(PyOmexMetaAPI.singular_annotation_get_about(self._obj))

    def __str__(self):
        return self.to_string("rdfxml-abbrev")

    def to_string(self, format: str, base_uri: str = "./Annotations.rdf") -> str:
        return PyOmexMetaAPI.get_and_free_c_str(
            PyOmexMetaAPI.singular_annotation_str(self._obj, format.encode(), base_uri.encode()))

    def get_predicate(self) -> str:
        return PyOmexMetaAPI.get_and_free_c_str(
            PyOmexMetaAPI.singular_annotation_get_predicate(self._obj))

    def get_resource(self) -> str:
        return PyOmexMetaAPI.get_and_free_c_str(
            PyOmexMetaAPI.singular_annotation_get_resource(self._obj))

    def delete(self) -> None:
        PyOmexMetaAPI.singular_annotation_delete(self._obj)


class PhysicalEntity:

    def __init__(self, physical_entity_ptr: ct.c_int64):
        self._obj = physical_entity_ptr

    def get_ptr(self) -> ct.c_int64:
        return self._obj

    def set_about(self, about: str) -> PhysicalEntity:
        self._obj = PyOmexMetaAPI.physical_entity_set_about(self._obj, about.encode())
        return self

    def set_physical_property(self, about: str, property: str) -> PhysicalEntity:
        self._obj = PyOmexMetaAPI.physical_entity_set_physical_property(self.get_ptr(), about.encode(), property.encode())
        return self

    def set_identity(self, identity: str) -> PhysicalEntity:
        self._obj = PyOmexMetaAPI.physical_entity_set_identity(self.get_ptr(), identity.encode())
        return self

    def add_location(self, location: str) -> PhysicalEntity:
        self._obj = PyOmexMetaAPI.physical_entity_add_location(self.get_ptr(), location.encode())
        return self

    def get_about(self) -> str:
        return PyOmexMetaAPI.get_and_free_c_str(PyOmexMetaAPI.physical_entity_get_about(self.get_ptr()))

    def get_physical_property(self) -> str:
        return PyOmexMetaAPI.get_and_free_c_str(PyOmexMetaAPI.physical_entity_get_physical_property(self.get_ptr()))

    def get_identity(self) -> str:
        return PyOmexMetaAPI.get_and_free_c_str(PyOmexMetaAPI.physical_entity_get_identity(self.get_ptr()))

    def get_num_locations(self) -> int:
        return PyOmexMetaAPI.physical_entity_get_num_locations(self.get_ptr())

    def get_locations(self) -> List[str]:
        return [PyOmexMetaAPI.get_and_free_c_str(
            PyOmexMetaAPI.physical_entity_get_location(self.get_ptr(), i)
        ) for i in range(self.get_num_locations())]

    def to_string(self, format: str, base_uri: str = "./Annotations.rdf") -> str:
        return PyOmexMetaAPI.get_and_free_c_str(
            PyOmexMetaAPI.physical_entity_str(self.get_ptr(), format.encode(), base_uri.encode())
        )

    def __str__(self):
        return self.to_string("rdfxml-abbrev")

    def delete(self) -> None:
        PyOmexMetaAPI.physical_entity_delete(self._obj)


class PhysicalProcess:

    def __init__(self, physical_process_ptr: ct.c_int64):
        self._obj = physical_process_ptr

    def get_ptr(self) -> ct.c_int64:
        return self._obj

    def set_about(self, about: str) -> PhysicalProcess:
        self._obj = PyOmexMetaAPI.physical_process_set_about(self._obj, about.encode())
        return self

    def set_physical_property(self, about: str, property: str) -> PhysicalProcess:
        self._obj = PyOmexMetaAPI.physical_process_set_physical_property(self._obj, about.encode(), property.encode())
        return self

    def add_source(self, multiplier: float, physical_entity_reference: str) -> PhysicalProcess:
        self._obj = PyOmexMetaAPI.physical_process_add_source(self._obj, multiplier,
                                                            physical_entity_reference.encode())
        return self

    def add_sink(self, multiplier: float, physical_entity_reference: str) -> PhysicalProcess:
        self._obj = PyOmexMetaAPI.physical_process_add_sink(self._obj, multiplier, physical_entity_reference.encode())
        return self

    def add_mediator(self, multiplier: float, physical_entity_reference: str) -> PhysicalProcess:
        self._obj = PyOmexMetaAPI.physical_process_add_mediator(self._obj, multiplier, physical_entity_reference.encode())
        return self

    def to_string(self, format: str, base_uri: str = "./Annotations.rdf"):
        return PyOmexMetaAPI.get_and_free_c_str(
            PyOmexMetaAPI.physical_process_str(self._obj, format.encode(), base_uri.encode()))

    def __str__(self):
        return self.to_string("rdfxml-abbrev")

    def get_about(self) -> str:
        return PyOmexMetaAPI.get_and_free_c_str(PyOmexMetaAPI.physical_process_get_about(self._obj))

    def delete(self) -> None:
        PyOmexMetaAPI.physical_process_delete(self._obj)


class PhysicalForce:

    def __init__(self, physical_force_ptr: ct.c_int64):
        self._obj = physical_force_ptr

    def get_ptr(self) -> ct.c_int64:
        return self._obj

    def set_about(self, about: str) -> PhysicalForce:
        self._obj = PyOmexMetaAPI.physical_force_set_about(self._obj, about.encode())
        return self

    def set_physical_property(self, about: str, property: str) -> PhysicalForce:
        self._obj = PyOmexMetaAPI.physical_force_set_physical_property(self._obj, about.encode(), property.encode())
        return self

    def add_source(self, multiplier: float, physical_entity_reference: str) -> PhysicalForce:
        self._obj = PyOmexMetaAPI.physical_force_add_source(self._obj, multiplier, physical_entity_reference.encode())
        return self

    def add_sink(self, multiplier: float, physical_entity_reference: str) -> PhysicalForce:
        self._obj = PyOmexMetaAPI.physical_force_add_sink(self._obj, multiplier,
                                                        physical_entity_reference.encode())
        return self

    def to_string(self, format: str, base_uri: str = "./Annotations.rdf"):
        return PyOmexMetaAPI.get_and_free_c_str(
            PyOmexMetaAPI.physical_force_str(self._obj, format.encode(), base_uri.encode()))

    def __str__(self):
        return self.to_string("rdfxml-abbrev")

    def get_about(self) -> str:
        return PyOmexMetaAPI.get_and_free_c_str(PyOmexMetaAPI.physical_force_get_about(self._obj))

    def get_physical_property(self) -> str:
        return PyOmexMetaAPI.get_and_free_c_str(
            PyOmexMetaAPI.physical_force_get_physical_property(self._obj))

    def delete(self) -> None:
        PyOmexMetaAPI.physical_force_delete(self._obj)