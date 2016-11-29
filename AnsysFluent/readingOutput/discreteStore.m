function store = discreteStore(store,num)

indices = linspace(1,length(store.time),num);

store.Ptime = [];
store.PQ1 = [];
store.PQ2 = [];
store.PP1 = [];
store.PP2 = [];

for it = 1:num
    
    index = floor(indices(it));
    time = store.time(index);
    Q1 = store.Q1(index);
    Q2 = store.Q2(index);
    P1 = store.P1(index);
    P2 = store.P2(index);
    
    store.Ptime(it) = time;
    store.PQ1(it) = Q1;
    store.PQ2(it) = Q2;
    store.PP1(it) = P1;
    store.PP2(it) = P2;
end

end