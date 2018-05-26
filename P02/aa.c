void hamiltonian(int i){
  int j;
  if(promising(i)){
    if(i==n-1)
      'escribir solucion'
    else:
      for (size_t j = 2; j < n; j++) {
        vindex[i+1] = j;
        hamiltonian(i+1);
      }
  }
}

int promising(int i){
  int j, correcta;
  correcta = 1;
  if((i==n-1) && !w[vindex[n-1][vindex[0]])
    correcta=0;
  else
    if(i>0 && !w[vindex[i-1][vindex[i]])
      correcta=0;
  j=1;

  while(correcta && j<i){
    if(vindex[i] == vindex[j])
      correcta=0;
    j++;
  }
  return correcta;
}
cairo_move_to(cr, x+w/2-w/6, y+w/2+(w/6*2));
cairo_line_to(cr, x+w-w/6, y+w/2+(w/6*2));
