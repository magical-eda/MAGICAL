Limbo.Bibtex {#Bibtex}
---------

[TOC]

# Introduction {#Bibtex_Introduction}

Scripts to convert bibtex format to jemdoc and latex formats. 
It also supports custom information in each bibentry. 

# Examples {#Bibtex_Examples}

Use simple.bib as input 
\include test/bibtex/benchmarks/simple.bib

## Jemdoc format for Webpages {#Bibtex_Jemdoc}

Run with python 
~~~~~~~~~~~~~~~~
python $LIMBO_DIR/include/limbo/bibtex/bibconvert.py --suffix web --highlight "Yibo Lin" --highlight "Lin, Yibo" --input simple.bib
~~~~~~~~~~~~~~~~

Output 
~~~~~~~~~~~~~~~~
= Publications


=== Journal Papers

=== Conference Papers

==== 2015


- \[C1\] David Z. Pan, Lars Liebmann, Bei Yu, Xiaoqing Xu and *Yibo Lin*, 
  "[http://dl.acm.org/citation.cfm?id=2744769.2747940 Pushing multiple patterning in sub-10nm: are we ready?]", 
  ACM/IEEE Design Automation Conference (DAC), San Francisco, CA, Jun 7-11, 2015. 
  ([yibolin_homepage/papers/C2_paper.pdf pdf])*(Invited Paper)*
~~~~~~~~~~~~~~~~

User can also specify multiple bibtex files as follows 
~~~~~~~~~~~~~~~~
--input file1.bib --input file2.bib
~~~~~~~~~~~~~~~~

## Latex format for Resume {#Bibtex_Latex}

Run with python 
~~~~~~~~~~~~~~~~
python $LIMBO_DIR/include/limbo/bibtex/bibconvert.py --suffix cv --highlight "Yibo Lin" --highlight "Lin, Yibo" --input simple.bib
~~~~~~~~~~~~~~~~

Output 
~~~~~~~~~~~~~~~~
\textbf{Journal Papers}
        

\begin{description}[font=\normalfont]
%{{{
    

%}}}
\end{description}
    

\textbf{Conference Papers}
        

\begin{description}[font=\normalfont]
%{{{
    

\item[{[C1]}]{
        David Z. Pan, Lars Liebmann, Bei Yu, Xiaoqing Xu and \textbf{Yibo Lin}, 
    ``\href{http://dl.acm.org/citation.cfm?id=2744769.2747940}{Pushing multiple patterning in sub-10nm: are we ready?}'', 
    ACM/IEEE Design Automation Conference (DAC), San Francisco, CA, Jun 7-11, 2015.
    (\textbf{Invited Paper})
}
        

%}}}
\end{description}
    

\end{rSection}

~~~~~~~~~~~~~~~~


# References {#Bibtex_References}

- [limbo/bibtex/bibconvert.py](@ref bibconvert)
