import java.util.NoSuchElementException;

/**
 * This class is Indexiable priority queue
 */

public class CarsMinPQ {

    private CarsObject[] cars ;
    private int[] pq;        // binary heap using 1-based indexing
    private int[] qp;        // inverse of pq - qp[pq[i]] = pq[qp[i]] = i
    private int N;
    private int capacity;
    private char mode;

    /**
     * 
     * @param capacity
     * @param mode
     */
    public CarsMinPQ(int capacity, char mode) {
        if(capacity < 0) throw new IllegalArgumentException("The size of the array is less than 0");
        this.capacity = capacity;
        this.mode = mode;
        N = 0;
        cars = new CarsObject[capacity + 1];
        pq   = new int[capacity + 1];
        qp   = new int[capacity + 1];                   
        for (int i = 0; i <= capacity; i++)
            qp[i] = -1;
    }
    
    /**
     * 
     * @return
     */
    public boolean is_empty(){
        return N == 0;
    }

    public int size(){
        return N;
    }

    public int get_min_pq(){
        if (N == 0) throw new NoSuchElementException("Priority queue is empty");
        return pq[1];
    }

	public CarsObject get_min() {
        if (N == 0) throw new NoSuchElementException("Priority queue is empty");
		return cars[pq[1]];
    }

    public int delete_min(){
        if (N == 0) throw new NoSuchElementException("Priority queue is empty");
        int min = pq[1];
        exch(1, N--);
        sink(1);
        assert min == pq[N+1];
        qp[min] = -1;        // delete
        cars[min] = null;    // to help with garbage collection
        pq[N+1] = -1;        // not needed
        return min;
    }

    /**
     * Is 'i' on the priority queue
     * @param i
     * @return
     */
    public boolean contains(int i) {
        validateIndex(i);
        return qp[i] != -1;
    }

    public void insert(int i, CarsObject new_car) {
        validateIndex(i);
        if(contains(i)) throw new IllegalArgumentException("The given index is already exists in the priority queue");
        N++;
        qp[i] = N;
        pq[N] = i;
        cars[i] = new_car;
        swim(N);
	}
    
    // 
    /**
     * Throw an IllegalArgumentException if i is an invalid index
     * @param i
     */
    private void validateIndex(int i) {
        if (i < 0) throw new IllegalArgumentException("index is negative: " + i);
        if (i >= capacity) throw new IllegalArgumentException("index >= capacity: " + i);
    }

    /***************************************************************************
    * General helper functions.
    ***************************************************************************/
    private boolean greater(int i, int j) {
        if(mode == 'p'){
            return cars[i].get_price() > cars[j].get_price();
        }

        if(mode == 'm'){
            return cars[i].get_mileage() > cars[i].get_mileage();
        }
        return false;
    }

    private void exch(int i, int j) {
        int swap = pq[i];
        pq[i] = pq[j];
        pq[j] = swap;
        qp[pq[i]] = i;
        qp[pq[j]] = j;
    }


   /***************************************************************************
    * Heap helper functions.
    ***************************************************************************/
    private void swim(int k) {
        while (k > 1 && greater(k/2, k)) {
            exch(k, k/2);
            k = k/2;
        }
    }

    private void sink(int k) {
        while (2*k <= N) {
            int j = 2*k;
            if (j < N && greater(j, j+1)) j++;
            if (!greater(k, j)) break;
            exch(k, j);
            k = j;
        }
    }

}
