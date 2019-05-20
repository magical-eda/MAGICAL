%
% [At,b,c,K]=readsdpa(fname)
%
% Reads in a problem in SDPA sparse format, and returns it in SeDuMi
% format.
%
% 7/20/07  Modified to handle comments and other cruft in the SDPA
% file.  In particular, 
%
%    1. Initial comment lines beginning with " or * are ignored.
%    2. In the first three lines, any extraneous characters after
%       the numbers are ignored.
%    3. In the block description, any occurrences of "{", ",", "}", "(",
%       and ")" are converted into spaces.
%
function [At,b,c,K]=readsdpa(fname)
%
% Open the file for reading.
%
fid=fopen(fname,'r');
if (fid == -1),
  fprintf('file does not exist!\n');
  return;
end;
%
% Skip over the initial comments.
%
inputline=fgetl(fid);
while (inputline(1)=='"' || inputline(1)=='*')
  inputline=fgetl(fid);
end
%
% Read in m.
%
m=sscanf(inputline,'%d',1);
inputline=fgetl(fid);
%
% Read in nblocks.
%
nblocks=sscanf(inputline,'%d',1);
inputline=fgetl(fid);
%
% Read in the raw block sizes.
%
inputline=regexprep(inputline,'[\,(){}]',' ');
rawblocksizes=sscanf(inputline,'%d',nblocks);
inputline=fgetl(fid);
%
% Compute the actual block sizes, figure out block types, and figure out
% where in the vectors stuff will go.
%
blocksizes=abs(rawblocksizes);
blocktypes=zeros(nblocks,1);
for i=1:nblocks,
  if (rawblocksizes(i) < 0)
     blocktypes(i)=1;
  else
     blocktypes(i)=2;
  end;
end;
blockbases=zeros(nblocks,1);
lpbase=1;
for i=1:nblocks,
  if (blocktypes(i)==1)
    blockbases(i)=lpbase;
    lpbase=lpbase+blocksizes(i);
  end;
end;
K.l=lpbase-1;
sdpbase=lpbase;
K.s=[];
for i=1:nblocks,
  if (blocktypes(i)==2)
    blockbases(i)=sdpbase;
    sdpbase=sdpbase+blocksizes(i)^2;
    K.s=[K.s blocksizes(i)];
  end;
end;
n=sdpbase-1;
%
% Now, read in the right hand side.
%
b=zeros(m,1);
inputline=regexprep(inputline,'[\,(){}]',' ');
b=sscanf(inputline,'%le',m);
%
% Now, read in the entries in the constraints and c.
%
c=zeros(1,n);
At=sparse(n,m);
[entries,count]=fscanf(fid,'%d %d %d %d %le',[5,inf]);
count=count/5;
[entriesm,entriesn]=size(entries);
for i=1:entriesn,
  if (entries(1,i)==0),
    block=entries(2,i);
    if (blocktypes(block)==1)
%
% LP data.
%
      c(1,blockbases(block)+entries(3,i)-1)=entries(5,i);
    else
%
% SDP block entry
%
      c(1,blockbases(block)+(entries(3,i)-1)*blocksizes(block)+ ...
        entries(4,i)-1)=entries(5,i);
      c(1,blockbases(block)+(entries(4,i)-1)*blocksizes(block)+ ...
        entries(3,i)-1)=entries(5,i);
    end;
  else
%
% Constraint entry.
%
    block=entries(2,i);
    constraint=entries(1,i);
    if (blocktypes(block)==1)
%
% LP data.
%
      At(blockbases(block)+entries(3,i)-1,constraint)=entries(5,i);
    else
%
% SDP block entry
%
      At(blockbases(block)+(entries(3,i)-1)*blocksizes(block)+ ...
        entries(4,i)-1,constraint)=entries(5,i);
      At(blockbases(block)+(entries(4,i)-1)*blocksizes(block)+ ...
        entries(3,i)-1,constraint)=entries(5,i);
    end;
  end;
end;
%
% Fix up the sign of c to match SeDuMi's convention.  Also, make c
% a column vector to match SeDuMi's fromsdpa(). 
%
c=-c';
