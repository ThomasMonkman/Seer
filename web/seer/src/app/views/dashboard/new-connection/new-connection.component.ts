import { Component, OnInit } from '@angular/core';
import { FormControl, FormGroup, Validators } from '@angular/forms';
import { ConnectionMeta } from 'app/classes/connnection-meta';
import { ConnectionHistoryService } from 'app/services/connection-history/connection-history.service';
import { Observable } from 'rxjs/Observable';

@Component({
  selector: 'app-new-connection',
  templateUrl: './new-connection.component.html',
  styleUrls: ['./new-connection.component.scss']
})
export class NewConnectionComponent implements OnInit {
  protected connectionForm = new FormGroup({
    address: new FormControl('', [Validators.required]),
    name: new FormControl(''),
  });

  protected filteredPastConnections: ConnectionMeta[] = [];

  constructor(private connectionHistory: ConnectionHistoryService) {
    connectionHistory.store(new ConnectionMeta('localhost', 'localhost'));
    // connectionHistory.get().subscribe((connections) => {
    //   this.pastConnections = connections;
    // });

    Observable.combineLatest(
      this.connectionHistory.get(),
      this.connectionForm.get('address').valueChanges.startWith(null),
      this.connectionForm.get('name').valueChanges.startWith(null)
    ).subscribe(this.filterHistory);
  }

  public ngOnInit() {
  }

  protected connect() {
    if (this.connectionForm.status === 'VALID') {
      console.log('connect');
    }
  }

  private filterHistory(val: [ConnectionMeta[], string, string]) {
    const [pastConnections, address, name] = val;
    this.filteredPastConnections = pastConnections.filter(connection => {
      let addressFilter = address ? connection.address.toLowerCase().includes(address.toLowerCase()) : true;
      let nameFilter = name ? connection.name.toLowerCase().includes(name.toLowerCase()) : true;
      return addressFilter || nameFilter;
    });
    console.log(this.filteredPastConnections);
  }
}
