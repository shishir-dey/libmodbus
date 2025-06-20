# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import os
import sys

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'libmodbus'
copyright = '2025, Shishir Dey'
author = 'Shishir Dey'
version = '1.0'
release = '1.0.0'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    'breathe',
    'myst_parser',
    'sphinx.ext.autodoc',
    'sphinx.ext.viewcode',
    'sphinx.ext.napoleon',
    'sphinx.ext.githubpages',
]

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']

# Theme options
html_theme_options = {
    'logo_only': False,
    'collapse_navigation': False,
    'sticky_navigation': True,
    'navigation_depth': 4,
    'includehidden': True,
    'titles_only': False,
    'style_external_links': True,
}

# -- Breathe configuration --------------------------------------------------

breathe_projects = {
    "libmodbus": "./doxygen/xml"
}
breathe_default_project = "libmodbus"
breathe_default_members = ('members', 'undoc-members')

# -- MyST-Parser configuration ----------------------------------------------

myst_enable_extensions = [
    "deflist",
    "tasklist",
    "colon_fence",
]

# -- Additional configuration -----------------------------------------------

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

# Add custom CSS if needed
html_css_files = []

# The name of the Pygments (syntax highlighting) style to use.
pygments_style = 'sphinx'

# If true, `todo` and `todoList` produce output, else they produce nothing.
todo_include_todos = False

# -- External links ---------------------------------------------------------

# GitHub repository URL
html_context = {
    "display_github": True,
    "github_user": "shishir-dey",
    "github_repo": "libmodbus",
    "github_version": "main",
    "conf_py_path": "/docs/",
} 